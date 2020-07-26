
#include "EngineApiBridge.hxx"

namespace chunklands::core {

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
    JS_CB(gbufferPassInit),
  }))

  JSValue
  EngineApiBridge::JSCall_GLFWInit(JSCbi info) {
    return FromNodeThreadRunApiResultInNodeThread(info.Env(), api_->GLFWInit(), [](JSEnv env, boost::future<void> result, JSDeferred deferred) {
      result.get();
      deferred.Resolve(env.Undefined());
    });
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

    return FromNodeThreadRunApiResultInNodeThread(info.Env(), api_->WindowCreate(width, height, std::move(title)), [](JSEnv env, boost::future<engine::WindowHandle*> result, JSDeferred deferred) {
      engine::WindowHandle* const window_handle = result.get();

      assert(window_handle);
      deferred.Resolve(JSExternal<engine::WindowHandle>::New(env, window_handle));
    });
  }

  JSValue
  EngineApiBridge::JSCall_windowLoadGL(JSCbi info) {
    JSExternal<engine::WindowHandle> js_window = info[0].As<JSExternal<engine::WindowHandle>>();
    return FromNodeThreadRunApiResultInNodeThread(info.Env(), api_->WindowLoadGL(js_window.Data()), [](JSEnv env, boost::future<void> result, JSDeferred deferred) {
      result.get();
      deferred.Resolve(env.Undefined());
    });
  }

  JSValue
  EngineApiBridge::JSCall_windowOn(JSCbi info) {
    JSExternal<engine::WindowHandle> js_window = info[0].As<JSExternal<engine::WindowHandle>>();
    
    std::string type = info[1].ToString();
    JSRef2 js_ref = JSRef2::New(info.Env(), info[2]);
    
    boost::signals2::scoped_connection conn = api_->WindowOn(js_window.Data(), std::move(type), [this, js_ref = std::move(js_ref)]() {
      
      struct data_t {
        data_t(JSRef2 ref) : ref(std::move(ref)) {}
        JSRef2 ref;
      };

      RunInNodeThread(std::make_unique<data_t>(std::move(js_ref)), [](JSEnv env, JSFunction, data_t* data_ptr) {
        std::unique_ptr<data_t> data(data_ptr);
        data->ref.Value().As<JSFunction>().Call(env.Null(), {});
      });
    });

    return JSFunction::New(info.Env(), [conn = conn.release()](JSCbi) {
      conn.disconnect();
    }, "cleanup", nullptr);
  }

  JSValue
  EngineApiBridge::JSCall_gbufferPassInit(JSCbi info) {
    JSExternal<engine::WindowHandle> js_window = info[0].As<JSExternal<engine::WindowHandle>>();
    std::string vertex_shader_source = info[1].ToString();
    std::string fragment_shader_source = info[2].ToString();
    return FromNodeThreadRunApiResultInNodeThread(info.Env(), api_->GBufferPassInit(js_window.Data(), vertex_shader_source, fragment_shader_source), [](JSEnv env, boost::future<void> result, JSDeferred deferred) {
      result.get();
      deferred.Resolve(env.Undefined());
    });
  }

} // namespace chunklands::core