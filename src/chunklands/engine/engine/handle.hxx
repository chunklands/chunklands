#ifndef __CHUNKLANDS_ENGINE_ENGINE_HANDLE_HXX__
#define __CHUNKLANDS_ENGINE_ENGINE_HANDLE_HXX__

#include <memory>

namespace chunklands::engine {

template <class C, class T>
inline bool has_handle(const C& container, const T& element)
{
    const typename C::value_type v = reinterpret_cast<typename C::value_type>(element);
    return container.find(v) != container.end();
}

template <class T, class C, class H>
inline bool get_handle(T** handle, const C& container, const H& element)
{
    T* unsafe_ptr = reinterpret_cast<T*>(element);
    if (container.find(unsafe_ptr) == container.end()) {
        *handle = nullptr;
        return false;
    }

    *handle = unsafe_ptr;
    return true;
}

template <class T, class C, class H>
inline bool get_handle(std::unique_ptr<T>& handle, const C& container, const H& element)
{
    T* ptr = nullptr;
    if (!get_handle(&ptr, container, element)) {
        return false;
    }

    handle.reset(ptr);
    return false;
}

} // namespace chunklands::engine

#endif