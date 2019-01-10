#include <napi.h>
#include "gl.h"

#include "WindowBase.h"
#include "SceneBase.h"
#include "GameLoopBase.h"

#define EXPORTS_CLASS(clazz) \
  do { \
    clazz::Initialize(env); \
    exports[#clazz] = clazz::constructor.Value(); \
  } while (0)

void Initialize(const Napi::CallbackInfo& info) {
  const int init = glfwInit();
  if (init != GLFW_TRUE) {
    Napi::Error::New(info.Env(), "could not initialize GLFW").ThrowAsJavaScriptException();
    return;
  }
}

void LoadProcs(const Napi::CallbackInfo& info) {
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

void Terminate(const Napi::CallbackInfo& info) {
  glfwTerminate();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    
  using namespace chunklands;

  exports["initialize"] = Napi::Function::New(env, Initialize);
  exports["terminate"] = Napi::Function::New(env, Terminate);
  exports["loadProcs"] = Napi::Function::New(env, LoadProcs);

  EXPORTS_CLASS(WindowBase);
  EXPORTS_CLASS(SceneBase);
  EXPORTS_CLASS(GameLoopBase);

  return exports;
}

NODE_API_MODULE(chunklands_core, Init)