#ifndef __CHUNKLANDS_CORE_RESOLVER_HXX__
#define __CHUNKLANDS_CORE_RESOLVER_HXX__

#include "handle.hxx"
#include <tuple>

namespace chunklands::core {

template <class T, class F, class... R>
inline auto create_resolver(std::tuple<R...> refs, F&& fn)
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
inline auto create_resolver(F&& fn)
{
    return create_resolver<T>(std::make_tuple(), std::forward<F>(fn));
}

inline JSValue none_resolver(JSEnv env, const engine::CENone&)
{
    return env.Undefined();
}

template <class T>
inline auto create_resolver()
{
    return create_resolver<T>(none_resolver);
}

template <class T>
inline JSValue handle_resolver(JSEnv env, const T& handle)
{
    return get_handle(env, handle);
}

} // namespace chunklands::core

#endif