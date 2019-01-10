#include "EnvironmentBase.h"
#include "gl.h"

namespace chunklands {
  
  Napi::FunctionReference EnvironmentBase::constructor;

  void EnvironmentBase::Initialize(Napi::Env env) {
    constructor = Napi::Persistent(DefineClass(env, "EnvironmentBase", {
      StaticMethod("initialize", Initialize_),
      StaticMethod("loadProcs", LoadProcs),
      StaticMethod("terminate", Terminate)
    }));
  }

  EnvironmentBase::EnvironmentBase(const Napi::CallbackInfo& info) : Napi::ObjectWrap<EnvironmentBase>(info) {

  }

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