
#include "_.hxx"
#include <chunklands/jsexport.hxx>
#include "ProgramBase.hxx"

namespace chunklands::gl {

  JSObject Init(JSEnv env, JSObject exports) {
    JS_EXPORT(ProgramBase);
    return exports;
  }

} // namespace chunklands::gl