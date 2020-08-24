
#include "EngineBridge.hxx"
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/easylogging++.hxx>
#include <tuple>

namespace chunklands::core {

template <class T, class F, class... R>
auto create_resolver(std::tuple<R...> refs, F&& fn)
{
    return [refs = std::move(refs), fn = std::move(fn)](JSEnv env, engine::EngineResultX<T> result, JSDeferred deferred) {
        if (result.IsError()) {
            JSError err = JSError::New(env, engine::get_engine_exception_message(result.Error()));
            deferred.Reject(err.Value());
            return;
        }

        deferred.Resolve(fn(env, result.Value()));
    };
}

template <class T, class F>
auto create_resolver(F&& fn)
{
    return create_resolver<T>(std::make_tuple(), std::forward<F>(fn));
}

JSValue none_resolver(JSEnv env, const engine::CENone&)
{
    return env.Undefined();
}

template <class T>
auto create_resolver()
{
    return create_resolver<T>(none_resolver);
}

template <class T>
bool unsafe_get_handle_ptr(T* ptr, JSEnv env, JSValue js_value)
{
    assert(ptr != nullptr);
    if (js_value.Type() != napi_bigint) {
        return false;
    }

    uint64_t result = 0;
    bool lossless = false;
    const napi_status status = napi_get_value_bigint_uint64(env, js_value, &result, &lossless);

    if (status != napi_ok) {
        return false;
    }

    if (!lossless) {
        return false;
    }

    *ptr = reinterpret_cast<T>(result);
    return true;
}

template <class T>
JSValue get_handle(JSEnv env, T* ptr)
{
    uint64_t handle = reinterpret_cast<uint64_t>(ptr);
    napi_value result;
    const napi_status status = napi_create_bigint_uint64(env, handle, &result);
    NAPI_THROW_IF_FAILED(env, status, JSNumber::New(env, 0));

    return JSValue(env, result);
}

template <class T>
JSValue handle_resolver(JSEnv env, const T& handle)
{
    return get_handle(env, handle);
}

EngineBridge::EngineBridge(JSCbi info)
    : JSObjectWrap<EngineBridge>(info)
{
    engine_ = new engine::Engine();
    fn_ = JSTSFunction::New(info.Env(), JSFunction::New(
                                            info.Env(), [](JSCbi) {}, "dummy"),
        "EngineBridgeCallback", 0, 1);

    node_thread_id_ = std::this_thread::get_id();
}

EngineBridge::~EngineBridge()
{
    delete engine_;
    engine_ = nullptr;
}

JS_DEF_INITCTOR(EngineBridge, ONE_ARG({
                                  JS_CB(terminate),
                                  JS_CB(startProfiling),
                                  JS_CB(stopProfiling),
                                  JS_CB(GLFWInit),
                                  JS_CB(GLFWStartPollEvents),
                                  JS_CB(windowCreate),
                                  JS_CB(windowLoadGL),
                                  JS_CB(windowOn),
                                  JS_CB(renderPipelineInit),
                                  JS_CB(blockCreate),
                                  JS_CB(blockBake),
                                  JS_CB(chunkCreate),
                                  JS_CB(chunkDelete),
                                  JS_CB(chunkUpdate),
                                  JS_CB(sceneAddChunk),
                                  JS_CB(sceneRemoveChunk),
                                  JS_CB(cameraAttachWindow),
                                  JS_CB(cameraDetachWindow),
                                  JS_CB(cameraGetPosition),
                                  JS_CB(cameraOn),
                              }))

template <class F>
inline auto WrapEngineCall(F&& fn)
{
    try {
        return fn();
    } catch (const engine::engine_exception& e) {
        FatalAbort(e);
    } catch (const engine::gl::gl_exception& e) {
        FatalAbort(e);
    }
}

void EngineBridge::JSCall_terminate(JSCbi)
{
    LOG_PROCESS("EngineBridge terminate");
    engine_->Terminate();
    fn_.Release();
}

void EngineBridge::JSCall_startProfiling(JSCbi)
{
    EASY_PROFILER_ENABLE;
    EASY_MAIN_THREAD;
    profiler::startListen();
}

void EngineBridge::JSCall_stopProfiling(JSCbi info)
{
    EASY_PROFILER_DISABLE;
    if (info[0].IsString()) {
        std::string filename = info[0].ToString();
        profiler::dumpBlocksToFile(filename.data());
    }
}

JSValue
EngineBridge::JSCall_GLFWInit(JSCbi info)
{
    return MakeEngineCall(info.Env(),
        engine_->GLFWInit(),
        create_resolver<engine::CENone>());
}

void EngineBridge::JSCall_GLFWStartPollEvents(JSCbi info)
{
    engine_->GLFWStartPollEvents(info[0].ToBoolean());
}

JSValue
EngineBridge::JSCall_windowCreate(JSCbi info)
{
    const int width = info[0].ToNumber();
    const int height = info[1].ToNumber();
    std::string title = info[2].ToString();

    return MakeEngineCall(info.Env(),
        engine_->WindowCreate(width, height, std::move(title)),
        create_resolver<engine::CEWindowHandle*>(handle_resolver<engine::CEWindowHandle*>));
}

JSValue
EngineBridge::JSCall_windowLoadGL(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr; // ERROR HERE
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());
    return MakeEngineCall(info.Env(),
        engine_->WindowLoadGL(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_windowOn(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());

    return EventHandler<engine::CEWindowEvent>(
        info.Env(), info[1], info[2], [this, handle](const std::string& type, auto cb) { return engine_->WindowOn(handle, type, std::move(cb)); }, [](const engine::CEWindowEvent& event, JSEnv env, JSObject js_event) {
      if (event.type == "shouldclose") {
        // nothing
      } else if (event.type == "click") {
        js_event["button"]  = JSNumber::New(env, event.click.button);
        js_event["action"]  = JSNumber::New(env, event.click.action);
        js_event["mods"]    = JSNumber::New(env, event.click.mods);
      } else if (event.type == "key") {
        js_event["key"]       = JSNumber::New(env, event.key.key);
        js_event["scancode"]  = JSNumber::New(env, event.key.scancode);
        js_event["action"]    = JSNumber::New(env, event.key.action);
        js_event["mods"]      = JSNumber::New(env, event.key.mods);
      } });
}

JSValue
EngineBridge::JSCall_renderPipelineInit(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());

    engine::CERenderPipelineInit init;
    JSObject js_init = info[1].ToObject();

    JSObject js_gbuffer = js_init.Get("gbuffer").ToObject();
    init.gbuffer.vertex_shader = js_gbuffer.Get("vertexShader").ToString();
    init.gbuffer.fragment_shader = js_gbuffer.Get("fragmentShader").ToString();

    JSObject js_lighting = js_init.Get("lighting").ToObject();
    init.lighting.vertex_shader = js_lighting.Get("vertexShader").ToString();
    init.lighting.fragment_shader = js_lighting.Get("fragmentShader").ToString();

    return MakeEngineCall(info.Env(),
        engine_->RenderPipelineInit(handle, std::move(init)),
        create_resolver<engine::CENone>());
}

engine::FaceType face_type_by_string(const std::string& type)
{
    if (type == "left") {
        return engine::FaceType::Left;
    }

    if (type == "right") {
        return engine::FaceType::Right;
    }

    if (type == "top") {
        return engine::FaceType::Top;
    }

    if (type == "bottom") {
        return engine::FaceType::Bottom;
    }

    if (type == "front") {
        return engine::FaceType::Front;
    }

    if (type == "back") {
        return engine::FaceType::Back;
    }

    return engine::FaceType::Unknown;
}

JSValue
EngineBridge::JSCall_blockCreate(JSCbi info)
{
    JSObject js_init = info[0].ToObject();

    std::string id = js_init.Get("id").ToString();
    bool opaque = js_init.Get("opaque").ToBoolean();
    JSObject js_faces = js_init.Get("faces").ToObject();
    JSValue js_texture = js_init.Get("texture");

    // texture
    std::vector<unsigned char> texture;
    if (js_texture.IsBuffer()) {
        JSBuffer<unsigned char> js_texture_buffer = js_texture.As<JSBuffer<unsigned char>>();
        texture.resize(js_texture_buffer.ByteLength());
        std::memcpy(texture.data(), js_texture_buffer.Data() + js_texture_buffer.ByteOffset(), js_texture_buffer.ByteLength());
    }

    // faces
    std::vector<engine::CEBlockFace> faces;

    const JSArray js_faces_names = js_faces.GetPropertyNames();
    const uint32_t face_names_length = js_faces_names.Length();
    faces.resize(face_names_length);

    for (uint32_t i = 0; i < face_names_length; i++) {
        JSValue js_face_name = js_faces_names.Get(i);
        const std::string face_name = js_face_name.ToString();
        JSArrayBuffer js_data = js_faces.Get(js_face_name).As<JSArrayBuffer>();

        const size_t byte_length = js_data.ByteLength();
        JS_ENGINE_CHECK(byte_length % sizeof(engine::CEVaoElementChunkBlock) == 0, info.Env(), JSValue());
        const int items = byte_length / sizeof(engine::CEVaoElementChunkBlock);

        std::vector<engine::CEVaoElementChunkBlock> data;
        data.resize(items);
        std::memcpy(data.data(), js_data.Data(), byte_length);

        faces.push_back({ .type = face_type_by_string(face_name),
            .data = std::move(data) });
    }

    engine::CEBlockCreateInit init = {
        .id = std::move(id),
        .opaque = opaque,
        .faces = std::move(faces),
        .texture = std::move(texture)
    };

    return MakeEngineCall(info.Env(),
        engine_->BlockCreate(std::move(init)),
        create_resolver<engine::CEBlockHandle*>(handle_resolver<engine::CEBlockHandle*>));
}

JSValue
EngineBridge::JSCall_blockBake(JSCbi info)
{
    return MakeEngineCall(info.Env(),
        engine_->BlockBake(),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_chunkCreate(JSCbi info)
{
    const int x = info[0].ToNumber();
    const int y = info[1].ToNumber();
    const int z = info[2].ToNumber();

    return MakeEngineCall(info.Env(),
        engine_->ChunkCreate(x, y, z),
        create_resolver<engine::CEChunkHandle*>(handle_resolver<engine::CEChunkHandle*>));
}

JSValue
EngineBridge::JSCall_chunkDelete(JSCbi info)
{
    engine::CEChunkHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());

    return MakeEngineCall(info.Env(),
        engine_->ChunkDelete(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_chunkUpdate(JSCbi info)
{
    engine::CEChunkHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());
    JS_ENGINE_CHECK(info[1].IsArrayBuffer(), info.Env(), JSValue());

    JSArrayBuffer js_blocks = info[1].As<JSArrayBuffer>();
    JS_ENGINE_CHECK(js_blocks.ByteLength() == engine::CE_CHUNK_BLOCK_COUNT * sizeof(engine::CEBlockHandle*), info.Env(), JSValue());
    engine::CEBlockHandle** blocks = reinterpret_cast<engine::CEBlockHandle**>(js_blocks.Data());

    JSRef2 js_blocks_ref = JSRef2::New(info.Env(), js_blocks);
    return MakeEngineCall(info.Env(),
        engine_->ChunkUpdateData(handle, blocks),
        create_resolver<engine::CENone>(std::make_tuple(std::move(js_blocks_ref)),
            none_resolver));
}

JSValue
EngineBridge::JSCall_sceneAddChunk(JSCbi info)
{
    engine::CEChunkHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());

    return MakeEngineCall(info.Env(),
        engine_->SceneAddChunk(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_sceneRemoveChunk(JSCbi info)
{
    engine::CEChunkHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());

    return MakeEngineCall(info.Env(),
        engine_->SceneRemoveChunk(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_cameraAttachWindow(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());

    return MakeEngineCall(info.Env(),
        engine_->CameraAttachWindow(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_cameraDetachWindow(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());

    return MakeEngineCall(info.Env(),
        engine_->CameraDetachWindow(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_cameraGetPosition(JSCbi info)
{
    return MakeEngineCall(info.Env(),
        engine_->CameraGetPosition(),
        create_resolver<engine::CECameraPosition>([](JSEnv env, const engine::CECameraPosition& pos) {
            JSObject js_result = JSObject::New(env);
            js_result["x"] = JSNumber::New(env, pos.x);
            js_result["y"] = JSNumber::New(env, pos.y);
            js_result["z"] = JSNumber::New(env, pos.z);

            return js_result;
        }));
}

JSValue
EngineBridge::JSCall_cameraOn(JSCbi info)
{
    return EventHandler<engine::CECameraEvent>(
        info.Env(), info[0], info[1], [this](const std::string& type, auto cb) { return engine_->CameraOn(type, std::move(cb)); }, [](const engine::CECameraEvent& event, JSEnv, JSObject js_event) {
      if (event.type == "positionchange") {
        js_event["x"] = event.positionchange.x;
        js_event["y"] = event.positionchange.y;
        js_event["z"] = event.positionchange.z;
      } });
}

} // namespace chunklands::core