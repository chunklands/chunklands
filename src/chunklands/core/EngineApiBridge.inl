
#include "EngineApiBridge.hxx"

#include <chunklands/engine/engine_exception.hxx>
#include <sstream>

namespace chunklands::core {

  template<class T, class F>
  void EngineApiBridge::RunInNodeThread(std::unique_ptr<T> data, F&& fn) {
    assert(NotIsNodeThread());

    const napi_status status = fn_.NonBlockingCall(data.get(), std::forward<F>(fn));
    if (status == napi_ok) {
      data.release();
    }
  }

  template<class T>
  struct X {
    X(boost::future<T> result) : result(std::move(result)) {}

    boost::future<T> result;
  };

  template<class T, class F>
  JSPromise EngineApiBridge::FromNodeThreadRunApiResultInNodeThread(JSEnv env, boost::future<T> result, F fn) {
    assert(IsNodeThread());

    JSDeferred deferred = JSDeferred::New(env);
    JSPromise promise = deferred.Promise();
    result.then([this, deferred = std::move(deferred), fn = std::move(fn)](boost::future<T> result) {
      assert(NotIsNodeThread());

      std::unique_ptr<X<T>> data = std::make_unique<X<T>>(std::move(result));
      
      const napi_status status = fn_.NonBlockingCall(data.get(), [this, deferred = std::move(deferred), fn = std::move(fn)](JSEnv env, JSFunction, X<T>* data_ptr) {
        assert(IsNodeThread());

        std::unique_ptr<X<T>> data(data_ptr);
        try {
          fn(env, std::move(data->result), std::move(deferred));
        } catch (const engine::engine_exception& e) {
          std::ostringstream ss;
          backward::Printer printer;
          printer.object = true;
          printer.color_mode = backward::ColorMode::always;
          printer.address = false;
          printer.print(*boost::get_error_info<engine::traced>(e), ss);
          const std::string message = ss.str();
          Napi::Error::Fatal(__FILE__, message.data());
        }
      });

      if (status == napi_ok) {
        data.release();
      }
    });

    return promise;
  }

  template<class T, class F, class R>
  inline JSValue EngineApiBridge::RunInNodeThread(JSEnv env, boost::future<T> result, F&& fn) {
    JSDeferred deferred = JSDeferred::New(env);
    const JSPromise promise = deferred.Promise();
    RunInNodeThread(std::move(result), [deferred = std::move(deferred), fn = std::forward<F>(fn)](boost::future<T> result) {
      fn(std::move(result), std::move(deferred));
    });

    return promise;
  }

} // namespace chunklands::core