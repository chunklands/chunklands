#include <chunklands/js.hxx>
#include <chunklands/init.hxx>

using namespace chunklands;

JSObject Init(JSEnv env, JSObject exports) {
  chunklands::modules::Init(env, exports);
  return exports;
}

NODE_API_MODULE(chunklands, Init)
