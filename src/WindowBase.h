#ifndef __CHUNKLANDS_CORE_WINDOWBASE_H__
#define __CHUNKLANDS_CORE_WINDOWBASE_H__

#include <napi.h>
#include "gl.h"

namespace chunklands {

  class WindowBase : public Napi::ObjectWrap<WindowBase> {
  public:
    static Napi::FunctionReference constructor;
    static void Initialize(Napi::Env env);

  public: // JS
    WindowBase(const Napi::CallbackInfo& info);

    void MakeContextCurrent(const Napi::CallbackInfo& info);
    Napi::Value ShouldClose(const Napi::CallbackInfo& info);
    void Close(const Napi::CallbackInfo& info);
    void SwapBuffers(const Napi::CallbackInfo& info);
    void Clear(const Napi::CallbackInfo& info);

    void SetKeyCallback(const Napi::CallbackInfo& info);

  public: // Native
    int GetKey(int key);

  private:
    GLFWwindow* window_ = nullptr;
    Napi::FunctionReference key_callback_;
  };

}

#endif