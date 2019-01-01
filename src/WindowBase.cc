#include "WindowBase.h"

namespace chunklands_core {
  
  Napi::FunctionReference WindowBase::constructor;

  void WindowBase::Initialize(Napi::Env env) {
    constructor = Napi::Persistent(
      DefineClass(env, "WindowBase", {
        InstanceMethod("makeContextCurrent", &WindowBase::MakeContextCurrent),
        InstanceAccessor("shouldClose", &WindowBase::ShouldClose, nullptr),
        InstanceMethod("close", &WindowBase::Close),
        InstanceMethod("swapBuffers", &WindowBase::SwapBuffers),
        InstanceMethod("clear", &WindowBase::Clear)
      })
    );

    constructor.SuppressDestruct();
  }

  WindowBase::WindowBase(const Napi::CallbackInfo& info) : Napi::ObjectWrap<WindowBase>(info) {
    auto&& config = info[0].ToObject();
    window_ = glfwCreateWindow(
      config.Get("width").ToNumber(),
      config.Get("height").ToNumber(),
      config.Get("title").ToString().Utf8Value().c_str(),
      nullptr,
      nullptr
    );
  }

  void WindowBase::MakeContextCurrent(const Napi::CallbackInfo& info) {
    glfwMakeContextCurrent(window_);
  }

  Napi::Value WindowBase::ShouldClose(const Napi::CallbackInfo& info) {
    if (!window_) {
      return Napi::Boolean::New(info.Env(), true);
    }

    const int should_close = glfwWindowShouldClose(window_);
    return Napi::Boolean::New(info.Env(), should_close == GLFW_TRUE);
  }

  void WindowBase::Close(const Napi::CallbackInfo& info) {
    if (!window_) {
      return;
    }

    glfwDestroyWindow(window_);
    window_ = nullptr;
  }

  void WindowBase::SwapBuffers(const Napi::CallbackInfo& info) {
    if (!window_) {
      return;
    }
    
    glfwSwapBuffers(window_);
  }

  void WindowBase::Clear(const Napi::CallbackInfo& info) {
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  
}