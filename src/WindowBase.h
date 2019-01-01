#ifndef __CHUNKLANDS_CORE_WINDOWBASE_H__
#define __CHUNKLANDS_CORE_WINDOWBASE_H__

#include <napi.h>
#include <GLFW/glfw3.h>

namespace chunklands_core {

  class WindowBase : public Napi::ObjectWrap<WindowBase> {
  public:
    static Napi::FunctionReference constructor;
    static void Initialize(Napi::Env env);

  public:
    WindowBase(const Napi::CallbackInfo& info);

    void MakeContextCurrent(const Napi::CallbackInfo& info);
    Napi::Value ShouldClose(const Napi::CallbackInfo& info);
    void Close(const Napi::CallbackInfo& info);
    void SwapBuffers(const Napi::CallbackInfo& info);
    void Clear(const Napi::CallbackInfo& info);

    void SetKeyCallback(const Napi::CallbackInfo& info);

  private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void KeyCallback(int key, int scancode, int action, int mods);

  private:
    GLFWwindow* window_ = nullptr;
    Napi::FunctionReference key_callback_;
  };

}

#endif