#include <napi.h>
#include <GLFW/glfw3.h>

#include "WindowBase.h"

#define EXPORTS_CLASS(clazz) \
  do { \
    clazz::Initialize(env); \
    exports[#clazz] = clazz::constructor.Value(); \
  } while (0)

void Initialize(const Napi::CallbackInfo& info) {
  const int init = glfwInit();
  if (init != GLFW_TRUE) {
    Napi::Error::New(info.Env(), "could not initialize GLFW").ThrowAsJavaScriptException();
  }
}

void Terminate(const Napi::CallbackInfo& info) {
  glfwTerminate();
}

void PollEvents(const Napi::CallbackInfo& info) {
  glfwPollEvents();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    
  using namespace chunklands_core;

  exports["initialize"] = Napi::Function::New(env, Initialize);
  exports["terminate"] = Napi::Function::New(env, Terminate);
  exports["pollEvents"] = Napi::Function::New(env, PollEvents);

  EXPORTS_CLASS(WindowBase);

  return exports;
}

NODE_API_MODULE(chunklands_core, Init)