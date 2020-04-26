#include "EnvironmentBase.h"
#include "gl.h"

namespace chunklands {
  JS_DEF_WRAP(EnvironmentBase)

  void EnvironmentBase::JSCall_initialize(JSCbi info) {
    auto&& env = info.Env();
    const int init = glfwInit();
    JS_ASSERT_MSG(init == GLFW_TRUE, "could not initialize GLFW");
  }

  void EnvironmentBase::JSCall_loadProcs(JSCbi info) {
    auto&& env = info.Env();

    JS_ASSERT_MSG(glfwGetCurrentContext() != nullptr, "call after `window.makeCurrentContext()`");

    const int load = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    JS_ASSERT_MSG(load != 0, "could not load GL procs");
    
    CHECK_GL();
  }

  void EnvironmentBase::JSCall_terminate(JSCbi) {
    glfwTerminate();
  }
}