#ifndef __CHUNKLANDS_CORE_HANDLE_HXX__
#define __CHUNKLANDS_CORE_HANDLE_HXX__

#include <chunklands/libcxx/js.hxx>

namespace chunklands::core {

template <class T>
bool unsafe_get_handle_ptr(T* ptr, JSEnv env, JSValue js_value)
{
    assert(ptr != nullptr);
    if (js_value.Type() != napi_bigint) {
        return false;
    }

    uint64_t result = 0;
    bool lossless = false;
    const napi_status status = napi_get_value_bigint_uint64(env, js_value, &result, &lossless);

    if (status != napi_ok) {
        return false;
    }

    if (!lossless) {
        return false;
    }

    *ptr = reinterpret_cast<T>(result);
    return true;
}

template <class T>
JSValue get_handle(JSEnv env, T* ptr)
{
    uint64_t handle = reinterpret_cast<uint64_t>(ptr);
    napi_value result;
    const napi_status status = napi_create_bigint_uint64(env, handle, &result);
    NAPI_THROW_IF_FAILED(env, status, JSNumber::New(env, 0));

    return JSValue(env, result);
}

} // namespace chunklands::core

#endif