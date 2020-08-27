
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

template <class T, class F, class FC>
JSPromise EngineBridge::MakeEngineCall(JSEnv env, engine::AsyncEngineResult<T> async_result, F&& fn, FC&& cleanup)
{
    assert(IsNodeThread());

    // TODO(daaitch): needs ref?
    JSDeferred deferred = JSDeferred::New(env);
    JSPromise promise = deferred.Promise();

    async_result.Future().then(
        boost::launch::sync,
        [this,
            deferred = std::move(deferred),
            fn = std::forward<F>(fn),
            cleanup = std::forward<FC>(cleanup)](engine::AsyncEngineResult<T> async_resolved_result) {
            auto data = std::make_unique<Holder<engine::AsyncEngineResult<T>>>(std::move(async_resolved_result));

            const napi_status status = fn_.NonBlockingCall(data.get(), [this, deferred = std::move(deferred), fn = std::move(fn), cleanup](JSEnv env, JSFunction, Holder<engine::AsyncEngineResult<T>>* data_ptr) {
                assert(IsNodeThread());

                DecrementJsCalls();
                LogJsCalls();

                {
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
                }

                cleanup();
            });

            if (status == napi_ok) {
                data.release();
                IncrementJsCalls();
                LogJsCalls();
            } else {
                cleanup();
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

struct handler_data_t {
    handler_data_t(JSRef2 js_engine_bridge_ref, JSRef2 js_callback_ref)
        : js_engine_bridge_ref(std::move(js_engine_bridge_ref))
        , js_callback_ref(std::move(js_callback_ref))
    {
    }
    JSRef2 js_engine_bridge_ref;
    JSRef2 js_callback_ref;
};

template <class Event, class F, class F2>
JSValue EngineBridge::EventHandler(JSEnv env, JSValue js_type, JSValue js_callback, F&& fn_calls_engine, F2&& fn_result)
{
    std::string type = js_type.ToString();
    napi_status status = napi_reference_ref(env, *this, nullptr);
    NAPI_THROW_IF_FAILED(env, status, JSValue());

    napi_ref js_callback_ref = nullptr;
    status = napi_create_reference(env, js_callback, 1, &js_callback_ref);
    NAPI_THROW_IF_FAILED(env, status, JSValue());

    engine::EngineResultX<engine::EventConnection>
        result = fn_calls_engine(
            std::move(type),
            [this,
                js_callback_ref,
                fn_result = std::forward<F2>(fn_result)](Event&& event) {
                fn_.NonBlockingCall(
                    js_callback_ref,
                    [event = std::forward<Event>(event),
                        fn_result = std::move(fn_result)](JSEnv env, JSFunction, napi_ref js_callback_ref) {
                        napi_value js_callback_value;
                        napi_status status = napi_get_reference_value(env, js_callback_ref, &js_callback_value);
                        NAPI_THROW_IF_FAILED(env, status);

                        JSFunction js_callback(env, js_callback_value);

                        JSObject js_event = JSObject::New(env);
                        js_event["type"] = JSString::New(env, event.type);

                        fn_result(event, env, js_event);

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

    if (result.IsError()) {
        status = napi_reference_unref(env, *this, nullptr);
        NAPI_THROW_IF_FAILED(env, status, JSValue());

        status = napi_delete_reference(env, js_callback_ref);
        NAPI_THROW_IF_FAILED(env, status, JSValue());

        JSError err = JSError::New(env, engine::get_engine_exception_message(result.Error()));
        err.ThrowAsJavaScriptException();
        return JSValue();
    }

    return JSFunction::New(
        env, [conn = result.Value().release(), this_ref = (napi_ref)(*this), js_callback_ref](JSCbi info) {
            conn.disconnect();

            napi_status status;

            status = napi_reference_unref(info.Env(), this_ref, nullptr);
            NAPI_THROW_IF_FAILED(info.Env(), status, JSValue());

            status = napi_delete_reference(info.Env(), js_callback_ref);
            NAPI_THROW_IF_FAILED(info.Env(), status, JSValue());
        },
        "cleanup", nullptr);
}

} // namespace chunklands::core