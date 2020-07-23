#include <chunklands/js.hxx>
#include <chunklands/init.hxx>

using namespace chunklands;

JSObject InitModule(JSEnv env, JSObject exports) {
  chunklands::Init(env, exports);
  return exports;
}

NODE_API_MODULE(chunklands, InitModule)
