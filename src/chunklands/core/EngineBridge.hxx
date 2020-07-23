#ifndef __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__
#define __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__

#include <chunklands/core/js.hxx>
#include <chunklands/engine/Engine.hxx>
#include <memory>
#define BOOST_THREAD_VERSION 4
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_EXECUTORS
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/executors/loop_executor.hpp>
#include <boost/thread/executors/serial_executor.hpp>
#include <boost/thread/thread.hpp>

#include <iostream>

namespace chunklands::core {

  class EngineBridge : public JSObjectWrap<EngineBridge> {
    JS_IMPL_INITCTOR(EngineBridge, ONE_ARG({
      JS_CB(enginePollEvents),
      JS_CB(engineShutdown),
      JS_CB(windowCreate),
      JS_CB(windowOn),
      JS_CB(GLFWInit),
      JS_CB(GLFWStartPollEvents),
    }))

    JSValue JSCall_enginePollEvents(JSCbi info) {
      if (loop_.closed()) {
        return JSBoolean::New(info.Env(), false);
      }

      loop_.run_queued_closures();
      return JSBoolean::New(info.Env(), true);
    }

    void JSCall_engineShutdown(JSCbi) {
      loop_.close();
    }

    JSValue JSCall_GLFWInit(JSCbi info) {
      return RunInNodeThread(info.Env(), engine_->api->GLFWInit(), [](boost::future<bool> result, JSDeferred deferred) {
        if (result.get()) {
          deferred.Resolve(deferred.Env().Undefined());
        } else {
          deferred.Reject(deferred.Env().Undefined());
        }
      });
    }

    void JSCall_GLFWStartPollEvents(JSCbi info) {
      engine_->api->GLFWStartPollEvents(info[0].ToBoolean());
    }

    JSValue JSCall_windowCreate(JSCbi info) {
      const int width = info[0].ToNumber();
      const int height = info[1].ToNumber();
      std::string title = info[2].ToString();

      return RunInNodeThread(info.Env(), engine_->api->WindowCreate(width, height, std::move(title)), [](boost::future<engine::WindowHandle*> result, JSDeferred deferred) {
        engine::WindowHandle* const window_handle = result.get();
        if (!window_handle) {
          deferred.Reject(js_create_error(deferred.Env(), "could not create window").Value());
          return;
        }

        assert(window_handle);
        deferred.Resolve(JSExternal<engine::WindowHandle>::New(deferred.Env(), window_handle));
      });
    }

    JSValue JSCall_windowOn(JSCbi info) {
      JSExternal<engine::WindowHandle> js_window = info[0].As<JSExternal<engine::WindowHandle>>();
      assert(js_window.Data());
      
      std::string type = info[1].ToString();
      JSRef2 js_ref = JSRef2::New(info.Env(), info[2]);
      
      boost::signals2::scoped_connection conn = engine_->api->WindowOn(js_window.Data(), std::move(type), [this, js_ref = std::move(js_ref)]() {
        RunInNodeThread([js_ref = std::move(js_ref)]() {
          js_ref.Value().As<JSFunction>().Call(js_ref.Env().Null(), {});
        });
      });

      return JSFunction::New(info.Env(), [conn = conn.release()](JSCbi) {
        conn.disconnect();
      }, "cleanup", nullptr);
    }

  public:
    EngineBridge(JSCbi info) : JSObjectWrap<EngineBridge>(info), serial_(loop_) {
      engine_ = std::make_unique<engine::Engine>();
      engine_->Init();
    }

    ~EngineBridge() {
      engine_->Terminate();
    }

  private:
  
    template<class T, class F, class R = std::result_of_t<F&&(boost::future<T>)>>
    inline boost::future<R> RunInNodeThread(boost::future<T> result, F&& fn) {
      return result.then(serial_, std::forward<F>(fn));
    }

    template<class F>
    inline void RunInNodeThread(F&& fn) {
      serial_.submit(std::forward<F>(fn));
    }

    template<class T, class F, class R = std::result_of_t<F&&(boost::future<T>, JSDeferred)>>
    inline JSValue RunInNodeThread(JSEnv env, boost::future<T> result, F&& fn) {
      JSDeferred deferred = JSDeferred::New(env);
      const JSPromise promise = deferred.Promise();
      RunInNodeThread(std::move(result), [deferred = std::move(deferred), fn = std::forward<F>(fn)](boost::future<T> result) {
        fn(std::move(result), std::move(deferred));
      });

      return promise;
    }

  private:
    std::unique_ptr<engine::Engine> engine_;
    boost::loop_executor loop_;
    boost::serial_executor serial_;
  };

} // namespace chunklands::core

#endif