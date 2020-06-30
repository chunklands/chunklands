#ifndef __CHUNKLANDS_ENGINE_WINDOW_HXX__
#define __CHUNKLANDS_ENGINE_WINDOW_HXX__

#include <chunklands/js.hxx>
#include <chunklands/modules/gl/glfw.hxx>
#include <glm/vec2.hpp>
#include <boost/signals2.hpp>

namespace chunklands::engine {

  class Window : public JSObjectWrap<Window> {
    JS_IMPL_WRAP(Window, ONE_ARG({
      JS_CB(initialize),
      JS_CB(makeContextCurrent),
      JS_CB(shouldClose),
      JS_CB(close),
      JS_GETTER(GameControl),
      JS_SETTER(GameControl),
    }))

    JS_DECL_CB_VOID(initialize)
    JS_DECL_CB_VOID(makeContextCurrent)
    JS_DECL_CB(shouldClose)
    JS_DECL_CB_VOID(close)

  protected:
    JSValue JSCall_GetGameControl(JSCbi info) {
      return JSBoolean::New(info.Env(), game_control_);
    }

    void JSCall_SetGameControl(JSCbi info) {
      bool next_game_control = info[0].ToBoolean().Value();
      if (next_game_control == game_control_) {
        return;
      }

      if (next_game_control) {
        StartMouseGrab();
        game_control_ = true;
      } else {
        StopMouseGrab();
        game_control_ = false;
      }
    }

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
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

  public:
    boost::signals2::signal<void(int width, int height)> on_resize;
    boost::signals2::signal<void(double xdiff, double ydiff)> on_game_control_look;

    JSObjectRef events_;

  private:
    GLFWwindow* window_ = nullptr;
    bool game_control_ = false;
    glm::dvec2 game_control_last_cursor_pos_;
  };

} // namespace chunklands::engine

#endif