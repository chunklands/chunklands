#ifndef __CHUNKLANDS_LOG_H__
#define __CHUNKLANDS_LOG_H__

#include <glm/vec3.hpp>
#include <iostream>
#include <type_traits>

namespace std {
  template <typename T,
            typename = enable_if_t<is_scalar<decltype(T::x)>::value>,
            typename = enable_if_t<is_scalar<decltype(T::y)>::value>,
            typename = enable_if_t<is_scalar<decltype(T::z)>::value>
  >
  ostream& operator<<(ostream& os, const T& v) {
    return os << "{" << v.x << ", " << v.y << ", " << v.z << "}";
  }
}

#endif