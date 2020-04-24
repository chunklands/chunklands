#include "js.h"
#include "module_includes.inl"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    
  using namespace chunklands;
  #define XX(CLASSNAME) JS_MODULE_WRAPEXPORT(env, CLASSNAME);
  #include "module_exports.inl"
  #undef XX

  return exports;
}

NODE_API_MODULE(chunklands, Init)