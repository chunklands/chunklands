
#include "_.hxx"
#include <chunklands/jsexport.hxx>
#include "ProgramBase.hxx"
#include "Texture2.hxx"

namespace chunklands::gl {

  JSObject Init(JSEnv env, JSObject exports) {
    JS_EXPORT(ProgramBase);
    JS_EXPORT(Texture2);
    return exports;
  }

} // namespace chunklands::gl