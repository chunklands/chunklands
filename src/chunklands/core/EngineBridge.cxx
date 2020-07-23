
#include "EngineBridge.hxx"

namespace chunklands::core {

  EngineBridge::EngineBridge(JSCbi info) : JSObjectWrap<EngineBridge>(info) {
    engine_ = std::make_unique<engine::Engine>();
    engine_->Init();

    fn_ = JSTSFunction::New(info.Env(), JSFunction::New(info.Env(), [](JSCbi){}, "dummy"), "EngineBridgeCallback", 0, 1);
    node_thread_id_ = std::this_thread::get_id();
  }

  EngineBridge::~EngineBridge() {
    engine_->Terminate();
  }

  JS_DEF_INITCTOR(EngineBridge, ONE_ARG({
    JS_CB(engineShutdown),
    JS_CB(windowCreate),
    JS_CB(windowMakeContextCurrent),
    JS_CB(windowOn),
    JS_CB(GLFWInit),
    JS_CB(GLFWStartPollEvents),
  }))

  void
  EngineBridge::JSCall_engineShutdown(JSCbi) {
    fn_.Release();
  }

  JSValue
  EngineBridge::JSCall_GLFWInit(JSCbi info) {
    return FromNodeThreadRunApiResultInNodeThread(info.Env(), engine_->api->GLFWInit(), [](JSEnv env, std::unique_ptr<bool> result, JSDeferred deferred) {
      if (*result) {
        deferred.Resolve(env.Undefined());
      } else {
        deferred.Reject(env.Undefined());
      }
    });
  }

  void
  EngineBridge::JSCall_GLFWStartPollEvents(JSCbi info) {
    engine_->api->GLFWStartPollEvents(info[0].ToBoolean());
  }

  JSValue
  EngineBridge::JSCall_windowCreate(JSCbi info) {
    const int width = info[0].ToNumber();
    const int height = info[1].ToNumber();
    std::string title = info[2].ToString();

    return FromNodeThreadRunApiResultInNodeThread(info.Env(), engine_->api->WindowCreate(width, height, std::move(title)), [](JSEnv env, std::unique_ptr<engine::WindowHandle*> result, JSDeferred deferred) {
      if (!*result) {
        deferred.Reject(js_create_error(env, "could not create window").Value());
        return;
      }

      assert(*result);
      deferred.Resolve(JSExternal<engine::WindowHandle>::New(env, *result));
    });
  }

  void
  EngineBridge::JSCall_windowMakeContextCurrent(JSCbi info) {
    JSExternal<engine::WindowHandle> js_window = info[0].As<JSExternal<engine::WindowHandle>>();
    assert(js_window.Data());
    engine_->api->WindowMakeContextCurrent(js_window.Data());
  }

  JSValue
  EngineBridge::JSCall_windowOn(JSCbi info) {
      JSExternal<engine::WindowHandle> js_window = info[0].As<JSExternal<engine::WindowHandle>>();
      assert(js_window.Data());
      
      std::string type = info[1].ToString();
      JSRef2 js_ref = JSRef2::New(info.Env(), info[2]);
      
      boost::signals2::scoped_connection conn = engine_->api->WindowOn(js_window.Data(), std::move(type), [this, js_ref = std::move(js_ref)]() {
        
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

} // namespace chunklands::core