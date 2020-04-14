#ifndef __CHUNKLANDS_WINDOWBASE_H__
#define __CHUNKLANDS_WINDOWBASE_H__

#include <napi.h>
#include <boost/signals2.hpp>
#include <glm/vec2.hpp>
#include "gl.h"
#include "napi/object_wrap_util.h"

namespace chunklands {

  class WindowBase : public Napi::ObjectWrap<WindowBase> {
    DECLARE_OBJECT_WRAP(WindowBase)
    DECLARE_OBJECT_WRAP_CB(void MakeContextCurrent)
    DECLARE_OBJECT_WRAP_CB(Napi::Value ShouldClose)
    DECLARE_OBJECT_WRAP_CB(void Close)

  public:
    void SwapBuffers();

    int GetKey(int key);
    int GetMouseButton(int button);
    glm::dvec2 GetCursorPos() const;

    void StartMouseGrab();
    void StopMouseGrab();

    glm::ivec2 GetSize() const;

  private:
    void UpdateViewport(int width, int height);

  public:
    boost::signals2::signal<void(int width, int height)> on_resize;
    boost::signals2::signal<void(double xpos, double ypos)> on_cursor_move;

  private:
    GLFWwindow* window_ = nullptr;
  };

}

#endif