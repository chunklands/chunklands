#ifndef __CHUNKLANDS_CORE_WINDOWBASE_H__
#define __CHUNKLANDS_CORE_WINDOWBASE_H__

#include <napi.h>
#include <boost/signals2.hpp>
#include <glm/vec2.hpp>
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

  public: // Native
    void SwapBuffers();
    void Clear();
    int GetKey(int key);

    glm::ivec2 GetSize() const;

  private:
    void UpdateViewport(int width, int height);

  public:
    boost::signals2::signal<void(int width, int height)> on_resize;

  private:
    GLFWwindow* window_ = nullptr;
  };

}

#endif