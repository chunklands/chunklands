
#include "EngineApiBridge.hxx"
#include <chunklands/engine/api-types.hxx>

#define WRAP_API_CALL(CALL) WrapApiCall([&]() { return CALL; })

#define CHECK(x) do { if (!(x)) { Napi::Error::Fatal(__FILE__ ":" BOOST_PP_STRINGIZE(__LINE__), #x); } } while(0)

namespace chunklands::core {
  void api_call_void_resolver(JSEnv env, boost::future<void> result, JSDeferred deferred) {
    result.get();
    deferred.Resolve(env.Undefined());
  }

  template<class T>
  T* unsafe_get_handle_ptr(JSValue js_value) {
    CHECK(js_value.Type() == napi_bigint);
    uint64_t result;
    bool lossless;
    const napi_status status = napi_get_value_bigint_uint64(js_value.Env(), js_value, &result, &lossless);
    NAPI_THROW_IF_FAILED(js_value.Env(), status, nullptr);
    CHECK(lossless);

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

  EngineApiBridge::EngineApiBridge(JSCbi info) : JSObjectWrap<EngineApiBridge>(info) {
    js_engine_bridge_ = info[0];
    api_ = js_engine_bridge_->GetEngine().GetApi();
    assert(api_);

    fn_ = js_engine_bridge_->GetJSThreadSafeFunction();
    
    node_thread_id_ = std::this_thread::get_id();
  }

  EngineApiBridge::~EngineApiBridge() {
  }

  JS_DEF_INITCTOR(EngineApiBridge, ONE_ARG({
    JS_CB(GLFWInit),
    JS_CB(GLFWStartPollEvents),
    JS_CB(windowCreate),
    JS_CB(windowLoadGL),
    JS_CB(windowOn),
    JS_CB(renderPipelineInit),
    JS_CB(blockCreate),
    JS_CB(blockBake),
    JS_CB(chunkCreate),
    JS_CB(chunkUpdate),
    JS_CB(sceneAddChunk),
    JS_CB(sceneRemoveChunk),
    JS_CB(cameraAttachWindow),
    JS_CB(cameraDetachWindow),
  }))

  template<class F>
  inline auto WrapApiCall(F&& fn) {
    try {
      return fn();
    } catch (const engine::engine_exception& e) {
      FatalAbort(e);
    } catch (const engine::gl::gl_exception& e) {
      FatalAbort(e);
    }
  }

  JSValue
  EngineApiBridge::JSCall_GLFWInit(JSCbi info) {
    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->GLFWInit()), api_call_void_resolver);
  }

  void
  EngineApiBridge::JSCall_GLFWStartPollEvents(JSCbi info) {
    api_->GLFWStartPollEvents(info[0].ToBoolean());
  }

  JSValue
  EngineApiBridge::JSCall_windowCreate(JSCbi info) {
    const int width = info[0].ToNumber();
    const int height = info[1].ToNumber();
    std::string title = info[2].ToString();

    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->WindowCreate(width, height, std::move(title))), [](JSEnv env, boost::future<engine::CEWindowHandle*> result, JSDeferred deferred) {
      engine::CEWindowHandle* const window_handle = result.get();

      assert(window_handle);
      deferred.Resolve(get_handle(env, window_handle));
    });
  }

  JSValue
  EngineApiBridge::JSCall_windowLoadGL(JSCbi info) {
    engine::CEWindowHandle *handle = unsafe_get_handle_ptr<engine::CEWindowHandle>(info[0]);

    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->WindowLoadGL(handle)), api_call_void_resolver);
  }

  JSValue
  EngineApiBridge::JSCall_windowOn(JSCbi info) {
    engine::CEWindowHandle *handle = unsafe_get_handle_ptr<engine::CEWindowHandle>(info[0]);
    
    std::string type = info[1].ToString();
    JSRef2 js_ref = JSRef2::New(info.Env(), info[2]);
    
    boost::signals2::scoped_connection conn = WRAP_API_CALL(api_->WindowOn(handle, std::move(type), [this, js_ref = std::move(js_ref)](engine::CEWindowEvent event) {
      
      struct data_t {
        data_t(JSRef2 ref) : ref(std::move(ref)) {}
        JSRef2 ref;
      };

      RunInNodeThread(std::make_unique<data_t>(std::move(js_ref)), [event = std::move(event)](JSEnv env, JSFunction, data_t* data_ptr) {
        std::unique_ptr<data_t> data(data_ptr);
        JSFunction js_callback = data->ref.Value().As<JSFunction>();

        JSObject js_event = JSObject::New(env);
        js_event["type"] = JSString::New(env, event.type);
        
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

        js_callback.Call({js_event});
      });
    }));

    return JSFunction::New(info.Env(), [conn = conn.release()](JSCbi) {
      conn.disconnect();
    }, "cleanup", nullptr);
  }

  JSValue
  EngineApiBridge::JSCall_renderPipelineInit(JSCbi info) {
    engine::CEWindowHandle *handle = unsafe_get_handle_ptr<engine::CEWindowHandle>(info[0]);
    
    engine::CERenderPipelineInit init;
    JSObject js_init = info[1].ToObject();

    JSObject js_gbuffer = js_init.Get("gbuffer").ToObject();
    init.gbuffer.vertex_shader = js_gbuffer.Get("vertexShader").ToString();
    init.gbuffer.fragment_shader = js_gbuffer.Get("fragmentShader").ToString();

    JSObject js_lighting = js_init.Get("lighting").ToObject();
    init.lighting.vertex_shader = js_lighting.Get("vertexShader").ToString();
    init.lighting.fragment_shader = js_lighting.Get("fragmentShader").ToString();

    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->RenderPipelineInit(handle, std::move(init))), api_call_void_resolver);
  }

  JSValue
  EngineApiBridge::JSCall_blockCreate(JSCbi info) {
    JSObject js_init = info[0].ToObject();
    std::string id = js_init.Get("id").ToString();
    bool opaque = js_init.Get("opaque").ToBoolean();
    JSArrayBuffer js_data = js_init.Get("data").As<JSArrayBuffer>();
    CHECK(js_data.ByteLength() % sizeof(engine::CEVaoElementChunkBlock) == 0);
    JSRef2 js_data_ref = JSRef2::New(info.Env(), js_data);  

    JSValue js_texture = js_init.Get("texture");  
    std::vector<unsigned char> texture;
    if (js_texture.IsBuffer()) {
      JSBuffer<unsigned char> js_texture_buffer = js_texture.As<JSBuffer<unsigned char>>();
      texture.resize(js_texture_buffer.ByteLength());
      std::memcpy(texture.data(), js_texture_buffer.Data() + js_texture_buffer.ByteOffset(), js_texture_buffer.ByteLength());
    }

    std::vector<engine::CEVaoElementChunkBlock> vao_elements;
    assert(js_data.ByteLength() % sizeof(engine::CEVaoElementChunkBlock) == 0);
    const std::size_t size = js_data.ByteLength() / sizeof(engine::CEVaoElementChunkBlock);
    vao_elements.resize(size);
    std::memcpy(vao_elements.data(), js_data.Data(), js_data.ByteLength());
    // const engine::CEVaoElementChunkBlock* data = reinterpret_cast<engine::CEVaoElementChunkBlock*>(js_data.Data());
    // for (int i = 0; i < js_data.ByteLength() / sizeof(engine::CEVaoElementChunkBlock); i++) {
    //   vao_elements.push_back(data[i]);
    // }
    assert(vao_elements.size() == size);
    engine::CEBlockCreateInit init = {
      .id = std::move(id),
      .opaque = opaque,
      .data = std::move(vao_elements),
      .texture = std::move(texture)
    };
    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->BlockCreate(std::move(init))), [js_data_ref = std::move(js_data_ref)](JSEnv env, boost::future<engine::CEBlockHandle*> result, JSDeferred deferred) {
      engine::CEBlockHandle* const handle = result.get();
      deferred.Resolve(get_handle(env, handle));
    });
  }

  JSValue
  EngineApiBridge::JSCall_blockBake(JSCbi info) {
    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->BlockBake()), api_call_void_resolver);
  }

  JSValue
  EngineApiBridge::JSCall_chunkCreate(JSCbi info) {
    CHECK(info[0].IsNumber());
    CHECK(info[1].IsNumber());
    CHECK(info[2].IsNumber());

    const int x = info[0].ToNumber();
    const int y = info[1].ToNumber();
    const int z = info[2].ToNumber();

    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->ChunkCreate(x, y, z)), [](JSEnv env, boost::future<engine::CEChunkHandle*> result, JSDeferred deferred) {
      engine::CEChunkHandle* const handle = result.get();
      deferred.Resolve(get_handle(env, handle));
    });
  }

  JSValue
  EngineApiBridge::JSCall_chunkUpdate(JSCbi info) {
    engine::CEChunkHandle* handle = unsafe_get_handle_ptr<engine::CEChunkHandle>(info[0]);
    CHECK(info[1].IsArrayBuffer());

    JSArrayBuffer js_blocks = info[1].As<JSArrayBuffer>();
    CHECK(js_blocks.ByteLength() == engine::CE_CHUNK_BLOCK_COUNT * sizeof(engine::CEBlockHandle*));
    engine::CEBlockHandle** blocks = reinterpret_cast<engine::CEBlockHandle**>(js_blocks.Data());

    JSRef2 js_blocks_ref = JSRef2::New(info.Env(), js_blocks);
    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->ChunkUpdateData(handle, blocks)), [js_blocks_ref = std::move(js_blocks_ref)](JSEnv env, boost::future<void> result, JSDeferred deferred) {
      result.get();
      deferred.Resolve(env.Undefined());
    });
  }

  JSValue
  EngineApiBridge::JSCall_sceneAddChunk(JSCbi info) {
    engine::CEChunkHandle* handle = unsafe_get_handle_ptr<engine::CEChunkHandle>(info[0]);

    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->SceneAddChunk(handle)), api_call_void_resolver);
  }

  JSValue
  EngineApiBridge::JSCall_sceneRemoveChunk(JSCbi info) {
    engine::CEChunkHandle* handle = unsafe_get_handle_ptr<engine::CEChunkHandle>(info[0]);

    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->SceneRemoveChunk(handle)), api_call_void_resolver);
  }

  JSValue
  EngineApiBridge::JSCall_cameraAttachWindow(JSCbi info) {
    engine::CEWindowHandle* handle = unsafe_get_handle_ptr<engine::CEWindowHandle>(info[0]);

    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->CameraAttachWindow(handle)), api_call_void_resolver);
  }

  JSValue
  EngineApiBridge::JSCall_cameraDetachWindow(JSCbi info) {
    engine::CEWindowHandle* handle = unsafe_get_handle_ptr<engine::CEWindowHandle>(info[0]);
    return FromNodeThreadRunApiResultInNodeThread(info.Env(), WRAP_API_CALL(api_->CameraDetachWindow(handle)), api_call_void_resolver);
  }

} // namespace chunklands::core