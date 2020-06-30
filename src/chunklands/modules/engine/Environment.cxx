
#include "Environment.hxx"

#include <chunklands/modules/gl/glfw.hxx>

namespace chunklands::modules::engine {

  JS_DEF_WRAP(Environment)

  void Environment::JSCall_initialize(JSCbi info) {
    const int init = glfwInit();
    JS_ASSERT_MSG(info.Env(), init == GLFW_TRUE, "could not initialize GLFW");
  }

  void Environment::JSCall_loadProcs(JSCbi info) {
    JS_ASSERT_MSG(info.Env(), glfwGetCurrentContext() != nullptr, "call after `window.makeCurrentContext()`");
    
    const int load = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
    JS_ASSERT_MSG(info.Env(), load != 0, "could not load GL procs");
    
    CHECK_GL();
  }

  void Environment::JSCall_terminate(JSCbi) {
    glfwTerminate();
  }

} // namespace chunklands::modules::engine