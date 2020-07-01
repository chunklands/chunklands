
#include "_.hxx"
#include <chunklands/jsexport.hxx>
#include "Profiler.hxx"

namespace chunklands::misc {

  JSObject Init(JSEnv env, JSObject exports) {
    JS_EXPORT(Profiler);
    return exports;
  }

} // namespace chunklands::misc