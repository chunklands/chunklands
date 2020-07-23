#ifndef __CHUNKLANDS_JSMATH_HXX__
#define __CHUNKLANDS_JSMATH_HXX__

// TODO(daaitch): maybe better jstransform.hxx ?

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "js.hxx"
#include "math.hxx"

namespace chunklands::jsmath {
  template<class T>
  math::vec2<T> vec2(JSArray js_array);
  template<class T>
  math::vec2<T> vec2(JSValue js_value);
  template<class T>
  JSArray vec2(JSEnv env, const math::vec2<T>& v);

  template<class T>
  math::vec3<T> vec3(JSArray js_array);
  template<class T>
  math::vec3<T> vec3(JSValue js_value);
  template<class T>
  JSArray vec3(JSEnv env, const math::vec3<T>& v);
}

#include "jsmath.inl"

#endif