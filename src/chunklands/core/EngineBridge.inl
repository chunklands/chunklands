
#include "EngineBridge.hxx"

#include <boost/signals2.hpp>
#include <chunklands/engine/engine_exception.hxx>
#include <chunklands/engine/gl/gl_exception.hxx>

namespace chunklands::core {

inline BOOST_NORETURN void FatalAbort(const engine::engine_exception& e)
{
    const std::string message = engine::get_engine_exception_message(e);
    Napi::Error::Fatal(__FILE__, message.data());
}

inline BOOST_NORETURN void FatalAbort(const engine::gl::gl_exception& e)
{
    const std::string message = engine::gl::get_gl_exception_message(e);
    Napi::Error::Fatal(__FILE__, message.data());
}

template <class T, class F>
void EngineBridge::RunInNodeThread(std::unique_ptr<T> data, F&& fn)
{

    const napi_status status = fn_.NonBlockingCall(data.get(), std::forward<F>(fn));
    if (status == napi_ok) {
        data.release();
    }
}

template <class T>
struct X {
    X(boost::future<T> result)
        : result(std::move(result))
    {
    }

    boost::future<T> result;
};

template <class T>
struct Holder {
    Holder(T&& data)
        : data(std::forward<T>(data))
    {
    }
    T data;
};

template <class F1, class F, class T>
JSPromise EngineBridge::MakeEngineCall(JSEnv env, F1&& engine_call, F fn)
{
    assert(IsNodeThread());

    auto result = WrapEngineCall(std::forward<F1>(engine_call));

    JSDeferred deferred = JSDeferred::New(env);
    JSPromise promise = deferred.Promise();
    result.then([this, deferred = std::move(deferred), fn = std::move(fn)](decltype(result) result) {
        assert(NotIsNodeThread());

        std::unique_ptr<X<T>> data = std::make_unique<X<T>>(std::move(result));

        const napi_status status = fn_.NonBlockingCall(data.get(), [this, deferred = std::move(deferred), fn = std::move(fn)](JSEnv env, JSFunction, X<T>* data_ptr) {
            assert(IsNodeThread());

            std::unique_ptr<X<T>> data(data_ptr);
            try {
                fn(env, std::move(data->result), std::move(deferred));
            } catch (const engine::engine_exception& e) {
                FatalAbort(e);
            } catch (const engine::gl::gl_exception& e) {
                FatalAbort(e);
            }
        });

        if (status == napi_ok) {
            data.release();
        }
    });

    return promise;
}

template <class T, class F>
JSPromise EngineBridge::MakeAsyncEngineCall(JSEnv env, engine::AsyncResult<T> async_result, F fn)
{
    assert(IsNodeThread());

    JSDeferred deferred = JSDeferred::New(env);
    JSPromise promise = deferred.Promise();

    async_result.then([this, deferred = std::move(deferred), fn = std::move(fn)](engine::AsyncResult<T> async_resolved_result) {
        assert(NotIsNodeThread());

        auto data = std::make_unique<Holder<engine::AsyncResult<T>>>(std::move(async_resolved_result));

        const napi_status status = fn_.NonBlockingCall(data.get(), [this, deferred = std::move(deferred), fn = std::move(fn)](JSEnv env, JSFunction, Holder<engine::AsyncResult<T>>* data_ptr) {
            assert(IsNodeThread());

            std::unique_ptr<Holder<engine::AsyncResult<T>>> data(data_ptr);
            try {
                auto result = data->data.get(); // may throw
                fn(env, std::move(result), std::move(deferred)); // may throw
            } catch (const engine::engine_exception& e) { // TODO(daaitch): no exceptions here ...
                FatalAbort(e);
            } catch (const engine::gl::gl_exception& e) {
                FatalAbort(e);
            } catch (...) {
                JSError::Fatal("unknown", "UNKNOWN ERROR");
            }
        });

        if (status == napi_ok) {
            data.release();
        }
    });

    return promise;
}

template <class T, class F>
JSPromise EngineBridge::MakeAsyncEngineCall2(JSEnv env, engine::AsyncEngineResult<T> async_result, F fn)
{
    assert(IsNodeThread());

    JSDeferred deferred = JSDeferred::New(env);
    JSPromise promise = deferred.Promise();

    async_result.Future().then([this, deferred = std::move(deferred), fn = std::move(fn)](engine::AsyncEngineResult<T> async_resolved_result) {
        assert(NotIsNodeThread());

        auto data = std::make_unique<Holder<engine::AsyncEngineResult<T>>>(std::move(async_resolved_result));

        const napi_status status = fn_.NonBlockingCall(data.get(), [this, deferred = std::move(deferred), fn = std::move(fn)](JSEnv env, JSFunction, Holder<engine::AsyncEngineResult<T>>* data_ptr) {
            assert(IsNodeThread());

            std::unique_ptr<Holder<engine::AsyncEngineResult<T>>> data(data_ptr);
            try {
                auto result = data->data.Future().get();
                fn(env, std::move(result), std::move(deferred)); // may throw
            } catch (const engine::engine_exception& e) { // TODO(daaitch): no exceptions here ...
                FatalAbort(e);
            } catch (const engine::gl::gl_exception& e) {
                FatalAbort(e);
            } catch (...) {
                JSError::Fatal("unknown", "UNKNOWN ERROR");
            }
        });

        if (status == napi_ok) {
            data.release();
        }
    });

    return promise;
}

template <class T, class F, class R>
inline JSValue EngineBridge::RunInNodeThread(JSEnv env, boost::future<T> result, F&& fn)
{
    JSDeferred deferred = JSDeferred::New(env);
    const JSPromise promise = deferred.Promise();
    RunInNodeThread(std::move(result), [deferred = std::move(deferred), fn = std::forward<F>(fn)](boost::future<T> result) {
        fn(std::move(result), std::move(deferred));
    });

    return promise;
}

struct event_handler_data_t {
    event_handler_data_t(JSRef2 ref)
        : ref(std::move(ref))
    {
    }
    JSRef2 ref;
};

template <class Event, class F, class F2>
JSValue EngineBridge::EventHandler(JSEnv env, JSValue js_type, JSValue js_callback, F&& fn_calls_engine, F2&& fn_result)
{
    std::string type = js_type.ToString();

    JSRef2 js_this_ref = JSRef2::New(env, Value());
    JSRef2 js_callback_ref = JSRef2::New(env, js_callback);

    engine::EventConnection conn = fn_calls_engine(
        std::move(type),
        [this,
            js_this_ref = std::move(js_this_ref),
            js_callback_ref = std::move(js_callback_ref),
            fn_result = std::forward<F2>(fn_result)](Event&& event) {
            RunInNodeThread(
                std::make_unique<event_handler_data_t>(js_callback_ref), // copy ref
                [event = std::forward<Event>(event),
                    fn_result // copy
            ](JSEnv env, JSFunction, event_handler_data_t* data_ptr) {
                    std::unique_ptr<event_handler_data_t> data(data_ptr);

                    JSObject js_event = JSObject::New(env);
                    js_event["type"] = JSString::New(env, event.type);

                    fn_result(event, env, js_event);

                    JSFunction js_callback = data->ref.Value().As<JSFunction>();
                    try {
                        js_callback.Call({ js_event });
                    } catch (const JSError& e) {
                        std::string stack = e.Get("stack").ToString();
                        std::cerr << e.Message() << "\n"
                                  << stack << std::endl;

                        // TODO(daaitch): js unhandled rejections handler
                        throw e;
                    }
                });
        });

    return JSFunction::New(
        env, [conn = conn.release()](JSCbi) {
            conn.disconnect();
        },
        "cleanup", nullptr);
}

} // namespace chunklands::core