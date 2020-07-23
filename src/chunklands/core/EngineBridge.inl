
#include "EngineBridge.hxx"

namespace chunklands::core {

  template<class T, class F>
  void EngineBridge::RunInNodeThread(std::unique_ptr<T> data, F&& fn) {
    assert(NotIsNodeThread());

    const napi_status status = fn_.NonBlockingCall(data.get(), std::forward<F>(fn));
    if (status == napi_ok) {
      data.release();
    }
  }

  template<class T, class F>
  JSPromise EngineBridge::FromNodeThreadRunApiResultInNodeThread(JSEnv env, boost::future<T> result, F fn) {
    assert(IsNodeThread());

    JSDeferred deferred = JSDeferred::New(env);
    JSPromise promise = deferred.Promise();

    result.then([this, deferred = std::move(deferred), fn = std::move(fn)](boost::future<T> result) {
      assert(NotIsNodeThread());

      std::unique_ptr<T> data = std::make_unique<T>(std::move(result.get()));
      
      const napi_status status = fn_.NonBlockingCall(data.get(), [this, deferred = std::move(deferred), fn = std::move(fn)](JSEnv env, JSFunction, T* data_ptr) {
        assert(IsNodeThread());

        std::unique_ptr<T> data(data_ptr);
        fn(env, std::move(data), std::move(deferred));
      });

      if (status == napi_ok) {
        data.release();
      }
    });

    return promise;
  }

  template<class T, class F, class R>
  inline JSValue EngineBridge::RunInNodeThread(JSEnv env, boost::future<T> result, F&& fn) {
    JSDeferred deferred = JSDeferred::New(env);
    const JSPromise promise = deferred.Promise();
    RunInNodeThread(std::move(result), [deferred = std::move(deferred), fn = std::forward<F>(fn)](boost::future<T> result) {
      fn(std::move(result), std::move(deferred));
    });

    return promise;
  }

} // namespace chunklands::core