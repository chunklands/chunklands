#include <chunklands/core/init.hxx>

using namespace chunklands;

JSObject InitModule(JSEnv env, JSObject exports) {
  chunklands::Init(env, exports);
  return exports;
}

NODE_API_MODULE(chunklands, InitModule)
