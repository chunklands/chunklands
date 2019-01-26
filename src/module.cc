#include <napi.h>

#include "napi/object_wrap_util.h"
#include "module_includes.inl"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    
  using namespace chunklands;
  #define XX(CLASSNAME) EXPORTS_OBJECT_WRAP(env, CLASSNAME);
  #include "module_exports.inl"
  #undef XX

  return exports;
}

NODE_API_MODULE(chunklands, Init)