#ifndef __CHUNKLANDS_WINDOWBASE_H__
#define __CHUNKLANDS_WINDOWBASE_H__

#include <boost/signals2.hpp>
#include <glm/vec2.hpp>
#include "gl.h"
#include "js.h"

namespace chunklands {

  class WindowBase : public JSWrap<WindowBase> {
    JS_DECL_WRAP(WindowBase)
    JS_DECL_CB_VOID(initialize)
    JS_DECL_CB_VOID(makeContextCurrent)
    JS_DECL_CB(shouldClose)
    JS_DECL_CB_VOID(close)

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