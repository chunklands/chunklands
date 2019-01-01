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
        InstanceMethod("clear", &WindowBase::Clear),
        InstanceMethod("setKeyCallback", &WindowBase::SetKeyCallback)
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

    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, KeyCallback);
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

  void WindowBase::SetKeyCallback(const Napi::CallbackInfo& info) {
    if (!info[0].IsFunction()) {
      key_callback_.Reset();
      return;
    }

    key_callback_ = Napi::Persistent(info[0].As<Napi::Function>());
  }

  void WindowBase::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowBase* target = reinterpret_cast<WindowBase*>(glfwGetWindowUserPointer(window));
    target->KeyCallback(key, scancode, action, mods);
  }

  void WindowBase::KeyCallback(int key, int scancode, int action, int mods) {
    if (key_callback_.IsEmpty()) {
      return;
    }

    Napi::HandleScope scope(Env());

    key_callback_.Call({
      Napi::Number::New(Env(), key),
      Napi::Number::New(Env(), scancode),
      Napi::Number::New(Env(), action),
      Napi::Number::New(Env(), mods)
    });
  }
  
}