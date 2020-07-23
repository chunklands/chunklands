#include <chunklands/js.hxx>
#include <chunklands/init.hxx>
#include <gtest/gtest.h>

using namespace chunklands;

JSValue Test(JSCbi info) {
  testing::InitGoogleTest();
  int result = RUN_ALL_TESTS();

  return JSBoolean::New(info.Env(), result == 0);
}

JSObject InitModule(JSEnv env, JSObject exports) {
  chunklands::Init(env, exports);
  exports["test"] = JSFunction::New(env, Test, "test");

  return exports;
}

NODE_API_MODULE(chunklands_test, InitModule)
