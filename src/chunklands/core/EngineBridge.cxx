
#include "EngineBridge.hxx"
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/easylogging++.hxx>

#define WRAP_ENGINE_CALL(CALL) [&]() { return CALL; }

#define CHECK_OR_FATAL(x) do { if (!(x)) { Napi::Error::Fatal(__FILE__ ":" BOOST_PP_STRINGIZE(__LINE__), #x); } } while(0)

namespace chunklands::core {
  void engine_call_void_resolver(JSEnv env, boost::future<void> result, JSDeferred deferred) {
    result.get();
    deferred.Resolve(env.Undefined());
  }

  template<class T>
  T* unsafe_get_handle_ptr(JSValue js_value) {
    CHECK_OR_FATAL(js_value.Type() == napi_bigint);
    uint64_t result;
    bool lossless;
    const napi_status status = napi_get_value_bigint_uint64(js_value.Env(), js_value, &result, &lossless);
    NAPI_THROW_IF_FAILED(js_value.Env(), status, nullptr);
    CHECK_OR_FATAL(lossless);

    return reinterpret_cast<T*>(result);
  }

  template<class T>
  JSValue get_handle(JSEnv env, T* ptr) {
    uint64_t handle = reinterpret_cast<uint64_t>(ptr);
    napi_value result;
    const napi_status status = napi_create_bigint_uint64(env, handle, &result);
    NAPI_THROW_IF_FAILED(env, status, JSNumber::New(env, 0));
    
    return JSValue(env, result);
  }

  EngineBridge::EngineBridge(JSCbi info) : JSObjectWrap<EngineBridge>(info) {
    engine_ = new engine::Engine();
    fn_ = JSTSFunction::New(info.Env(), JSFunction::New(info.Env(), [](JSCbi){}, "dummy"), "EngineBridgeCallback", 0, 1);
    
    node_thread_id_ = std::this_thread::get_id();
  }

  EngineBridge::~EngineBridge() {
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

  template<class F>
  inline auto WrapEngineCall(F&& fn) {
    try {
      return fn();
    } catch (const engine::engine_exception& e) {
      FatalAbort(e);
    } catch (const engine::gl::gl_exception& e) {
      FatalAbort(e);
    }
  }

  void EngineBridge::JSCall_terminate(JSCbi) {
    LOG_PROCESS("EngineBridge terminate");
    engine_->Terminate();
    fn_.Release();
  }


  void EngineBridge::JSCall_startProfiling(JSCbi) {
    EASY_PROFILER_ENABLE;
    EASY_MAIN_THREAD;
    profiler::startListen();
  }

  void EngineBridge::JSCall_stopProfiling(JSCbi info) {
    EASY_PROFILER_DISABLE;
    if (info[0].IsString()) {
      std::string filename = info[0].ToString();
      profiler::dumpBlocksToFile(filename.data());
    }
  }

  JSValue
  EngineBridge::JSCall_GLFWInit(JSCbi info) {
    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->GLFWInit()), engine_call_void_resolver);
  }

  void
  EngineBridge::JSCall_GLFWStartPollEvents(JSCbi info) {
    engine_->GLFWStartPollEvents(info[0].ToBoolean());
  }

  JSValue
  EngineBridge::JSCall_windowCreate(JSCbi info) {
    const int width = info[0].ToNumber();
    const int height = info[1].ToNumber();
    std::string title = info[2].ToString();

    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->WindowCreate(width, height, std::move(title))), [](JSEnv env, boost::future<engine::CEWindowHandle*> result, JSDeferred deferred) {
      engine::CEWindowHandle* const window_handle = result.get();

      assert(window_handle);
      deferred.Resolve(get_handle(env, window_handle));
    });
  }

  JSValue
  EngineBridge::JSCall_windowLoadGL(JSCbi info) {
    engine::CEWindowHandle *handle = unsafe_get_handle_ptr<engine::CEWindowHandle>(info[0]);

    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->WindowLoadGL(handle)), engine_call_void_resolver);
  }

  JSValue
  EngineBridge::JSCall_windowOn(JSCbi info) {
    engine::CEWindowHandle *handle = unsafe_get_handle_ptr<engine::CEWindowHandle>(info[0]);

    return EventHandler<engine::CEWindowEvent>(info.Env(), info[1], info[2], [this, handle](const std::string& type, auto cb) {
      return engine_->WindowOn(handle, type, std::move(cb));
    }, [](const engine::CEWindowEvent& event, JSEnv env, JSObject js_event) {
      if (event.type == "shouldclose") {
        // nothing
      }

      if (event.type == "click") {
        js_event["button"]  = JSNumber::New(env, event.click.button);
        js_event["action"]  = JSNumber::New(env, event.click.action);
        js_event["mods"]    = JSNumber::New(env, event.click.mods);
      }

      if (event.type == "key") {
        js_event["key"]       = JSNumber::New(env, event.key.key);
        js_event["scancode"]  = JSNumber::New(env, event.key.scancode);
        js_event["action"]    = JSNumber::New(env, event.key.action);
        js_event["mods"]      = JSNumber::New(env, event.key.mods);
      }
    });
  }

  JSValue
  EngineBridge::JSCall_renderPipelineInit(JSCbi info) {
    engine::CEWindowHandle *handle = unsafe_get_handle_ptr<engine::CEWindowHandle>(info[0]);
    
    engine::CERenderPipelineInit init;
    JSObject js_init = info[1].ToObject();

    JSObject js_gbuffer = js_init.Get("gbuffer").ToObject();
    init.gbuffer.vertex_shader = js_gbuffer.Get("vertexShader").ToString();
    init.gbuffer.fragment_shader = js_gbuffer.Get("fragmentShader").ToString();

    JSObject js_lighting = js_init.Get("lighting").ToObject();
    init.lighting.vertex_shader = js_lighting.Get("vertexShader").ToString();
    init.lighting.fragment_shader = js_lighting.Get("fragmentShader").ToString();

    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->RenderPipelineInit(handle, std::move(init))), engine_call_void_resolver);
  }

  engine::FaceType face_type_by_string(const std::string& type) {
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
  EngineBridge::JSCall_blockCreate(JSCbi info) {
    JSObject    js_init     = info[0].ToObject();
    
    std::string id          = js_init.Get("id").ToString();
    bool        opaque      = js_init.Get("opaque").ToBoolean();
    JSObject    js_faces    = js_init.Get("faces").ToObject();
    JSValue     js_texture  = js_init.Get("texture");

    // texture
    std::vector<unsigned char> texture;
    if (js_texture.IsBuffer()) {
      JSBuffer<unsigned char> js_texture_buffer = js_texture.As<JSBuffer<unsigned char>>();
      texture.resize(js_texture_buffer.ByteLength());
      std::memcpy(texture.data(), js_texture_buffer.Data() + js_texture_buffer.ByteOffset(), js_texture_buffer.ByteLength());
    }

    // faces
    std::vector<engine::CEBlockFace> faces;

    const JSArray js_faces_names      = js_faces.GetPropertyNames();
    const uint32_t face_names_length  = js_faces_names.Length();
    faces.resize(face_names_length);
    
    for (uint32_t i = 0; i < face_names_length; i++) {
      JSValue           js_face_name  = js_faces_names.Get(i);
      const std::string face_name     = js_face_name.ToString();
      JSArrayBuffer     js_data       = js_faces.Get(js_face_name).As<JSArrayBuffer>();
      
      const size_t byte_length = js_data.ByteLength();
      CHECK_OR_FATAL(byte_length % sizeof(engine::CEVaoElementChunkBlock) == 0);
      const int items = byte_length / sizeof(engine::CEVaoElementChunkBlock);

      std::vector<engine::CEVaoElementChunkBlock> data;
      data.resize(items);
      std::memcpy(data.data(), js_data.Data(), byte_length);

      faces.push_back({
        .type = face_type_by_string(face_name),
        .data = std::move(data)
      });
    }

    engine::CEBlockCreateInit init = {
      .id = std::move(id),
      .opaque = opaque,
      .faces = std::move(faces),
      .texture = std::move(texture)
    };

    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->BlockCreate(std::move(init))), [](JSEnv env, boost::future<engine::CEBlockHandle*> result, JSDeferred deferred) {
      engine::CEBlockHandle* const handle = result.get();
      deferred.Resolve(get_handle(env, handle));
    });
  }

  JSValue
  EngineBridge::JSCall_blockBake(JSCbi info) {
    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->BlockBake()), engine_call_void_resolver);
  }

  JSValue
  EngineBridge::JSCall_chunkCreate(JSCbi info) {
    CHECK_OR_FATAL(info[0].IsNumber());
    CHECK_OR_FATAL(info[1].IsNumber());
    CHECK_OR_FATAL(info[2].IsNumber());

    const int x = info[0].ToNumber();
    const int y = info[1].ToNumber();
    const int z = info[2].ToNumber();

    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->ChunkCreate(x, y, z)), [](JSEnv env, boost::future<engine::CEChunkHandle*> result, JSDeferred deferred) {
      engine::CEChunkHandle* const handle = result.get();
      deferred.Resolve(get_handle(env, handle));
    });
  }

  JSValue
  EngineBridge::JSCall_chunkDelete(JSCbi info) {
    engine::CEChunkHandle* const handle = unsafe_get_handle_ptr<engine::CEChunkHandle>(info[0]);
    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->ChunkDelete(handle)), engine_call_void_resolver);
  }

  JSValue
  EngineBridge::JSCall_chunkUpdate(JSCbi info) {
    engine::CEChunkHandle* handle = unsafe_get_handle_ptr<engine::CEChunkHandle>(info[0]);
    CHECK_OR_FATAL(info[1].IsArrayBuffer());

    JSArrayBuffer js_blocks = info[1].As<JSArrayBuffer>();
    CHECK_OR_FATAL(js_blocks.ByteLength() == engine::CE_CHUNK_BLOCK_COUNT * sizeof(engine::CEBlockHandle*));
    engine::CEBlockHandle** blocks = reinterpret_cast<engine::CEBlockHandle**>(js_blocks.Data());

    JSRef2 js_blocks_ref = JSRef2::New(info.Env(), js_blocks);
    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->ChunkUpdateData(handle, blocks)), [js_blocks_ref = std::move(js_blocks_ref)](JSEnv env, boost::future<void> result, JSDeferred deferred) {
      result.get();
      deferred.Resolve(env.Undefined());
    });
  }

  JSValue
  EngineBridge::JSCall_sceneAddChunk(JSCbi info) {
    engine::CEChunkHandle* handle = unsafe_get_handle_ptr<engine::CEChunkHandle>(info[0]);

    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->SceneAddChunk(handle)), engine_call_void_resolver);
  }

  JSValue
  EngineBridge::JSCall_sceneRemoveChunk(JSCbi info) {
    engine::CEChunkHandle* handle = unsafe_get_handle_ptr<engine::CEChunkHandle>(info[0]);

    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->SceneRemoveChunk(handle)), engine_call_void_resolver);
  }

  JSValue
  EngineBridge::JSCall_cameraAttachWindow(JSCbi info) {
    engine::CEWindowHandle* handle = unsafe_get_handle_ptr<engine::CEWindowHandle>(info[0]);

    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->CameraAttachWindow(handle)), engine_call_void_resolver);
  }

  JSValue
  EngineBridge::JSCall_cameraDetachWindow(JSCbi info) {
    engine::CEWindowHandle* handle = unsafe_get_handle_ptr<engine::CEWindowHandle>(info[0]);
    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->CameraDetachWindow(handle)), engine_call_void_resolver);
  }

  JSValue
  EngineBridge::JSCall_cameraGetPosition(JSCbi info) {
    return MakeEngineCall(info.Env(), WRAP_ENGINE_CALL(engine_->CameraGetPosition()), [](JSEnv env, boost::future<engine::CECameraPosition> result, JSDeferred deferred) {
      engine::CECameraPosition pos = result.get();
      JSObject js_result = JSObject::New(env);
      js_result["x"] = JSNumber::New(env, pos.x);
      js_result["y"] = JSNumber::New(env, pos.y);
      js_result["z"] = JSNumber::New(env, pos.z);

      deferred.Resolve(js_result);
    });
  }
  
  JSValue
  EngineBridge::JSCall_cameraOn(JSCbi info) {
    return EventHandler<engine::CECameraEvent>(info.Env(), info[0], info[1], [this](const std::string& type, auto cb) {
      return engine_->CameraOn(type, std::move(cb));
    }, [](const engine::CECameraEvent& event, JSEnv, JSObject js_event) {
      if (event.type == "positionchange") {
        js_event["x"] = event.positionchange.x;
        js_event["y"] = event.positionchange.y;
        js_event["z"] = event.positionchange.z;
      }
    });
  }

} // namespace chunklands::core