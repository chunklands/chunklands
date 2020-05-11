#include <chunklands/js.h>
#include <chunklands/modules/init.h>

using namespace chunklands;

JSObject Init(JSEnv env, JSObject exports) {
  chunklands::modules::Init(env, exports);
  return exports;
}

NODE_API_MODULE(chunklands, Init)
