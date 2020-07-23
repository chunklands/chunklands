
namespace chunklands::jsmath {

  template<class T>
  math::vec2<T> vec2(JSArray js_array) {
    JS_ASSERT(js_array.Env(), js_array.Length() == 2);
    
    return math::vec2<T>(
      (T)js_array.Get(js_index(0)).ToNumber(),
      (T)js_array.Get(js_index(1)).ToNumber()
    );
  }

  template<class T>
  math::vec2<T> vec2(JSValue js_value) {
    JS_ASSERT(js_value.Env(), js_value.IsArray());
    return vec2<T>(js_value.As<JSArray>());
  }

  template<class T>
  JSArray vec2(JSEnv env, const math::vec2<T>& v) {
    auto&& js_array = JSArray::New(env, 2);
    js_array[js_index(0)] = JSNumber::New(env, v.x);
    js_array[js_index(1)] = JSNumber::New(env, v.y);

    return js_array;
  }

  template<class T>
  math::vec3<T> vec3(JSArray js_array) {
    JS_ASSERT(js_array.Env(), js_array.Length() == 3);
    
    return glm::vec3(
      (T)js_array.Get(js_index(0)).ToNumber(),
      (T)js_array.Get(js_index(1)).ToNumber(),
      (T)js_array.Get(js_index(2)).ToNumber()
    );
  }

  template<class T>
  math::vec3<T> vec3(JSValue js_value) {
    JS_ASSERT(js_value.Env(), js_value.IsArray());
    return vec3<T>(js_value.As<JSArray>());
  }

  template<class T>
  JSArray vec3(JSEnv env, const math::vec3<T>& v) {
    auto&& js_array = JSArray::New(env, 3);
    js_array[js_index(0)] = JSNumber::New(env, v.x);
    js_array[js_index(1)] = JSNumber::New(env, v.y);
    js_array[js_index(2)] = JSNumber::New(env, v.z);

    return js_array;
  }
}