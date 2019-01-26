#include "EnvironmentBase.h"
#include "gl.h"

namespace chunklands {
  DEFINE_OBJECT_WRAP_DEFAULT_CTOR(EnvironmentBase, ONE_ARG({
    StaticMethod("initialize", Initialize_),
    StaticMethod("loadProcs", LoadProcs),
    StaticMethod("terminate", Terminate)
  }))

  void EnvironmentBase::Initialize_(const Napi::CallbackInfo& info) {
    const int init = glfwInit();
    if (init != GLFW_TRUE) {
      Napi::Error::New(info.Env(), "could not initialize GLFW").ThrowAsJavaScriptException();
      return;
    }
  }

  void EnvironmentBase::LoadProcs(const Napi::CallbackInfo& info) {
    if (glfwGetCurrentContext() == nullptr) {
      Napi::Error::New(info.Env(), "call after `window.makeCurrentContext()`").ThrowAsJavaScriptException();
      return;
    }

    const int load = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!load) {
      Napi::Error::New(info.Env(), "could not load GL procs").ThrowAsJavaScriptException();
      return;
    }
    
    CHECK_GL();
  }

  void EnvironmentBase::Terminate(const Napi::CallbackInfo& info) {
    glfwTerminate();
  }
}