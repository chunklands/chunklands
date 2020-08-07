#ifndef __CHUNKLANDS_ENGINE_WINDOW_HXX__
#define __CHUNKLANDS_ENGINE_WINDOW_HXX__

#include <chunklands/libcxx/glfw.hxx>
#include <boost/signals2.hpp>

namespace chunklands::engine {

  struct window_size_t {
    int width = 0;
    int height = 0;
  };

  struct window_cursor_move_t {
    double x = 0;
    double y = 0;
  };

  struct window_mouse_grab_t {
    double dx = 0;
    double dy = 0;
  };

  struct window_click_t {
    int button = 0;
    int action = 0;
    int mods = 0;
  };

  struct window_key_t {
    int key = 0;
    int scancode = 0;
    int action = 0;
    int mods = 0;
  };

  template<class T>
  struct slot_t {
    using value = boost::signals2::signal<void(const T&)>;
  };

  template<>
  struct slot_t<void> {
    using value = boost::signals2::signal<void()>;
  };

  template<class T>
  using slot = typename slot_t<T>::value;

  class Window {
  public:
    Window(GLFWwindow* glfw_window);
    ~Window();

    bool LoadGL();
    void SwapBuffers();
    bool IsGLLoaded() const {
      return is_gl_loaded_;
    }

    window_size_t GetSize() const;

    void StartMouseGrab();
    void StopMouseGrab();

  public:
    slot<void> on_close;
    slot<window_size_t>         on_resize;
    slot<window_cursor_move_t>  on_cursor_move;
    slot<window_click_t>        on_click;
    slot<window_mouse_grab_t>   on_mouse_grab;
    slot<window_key_t>          on_key;

  private:
    GLFWwindow* glfw_window_ = nullptr;
    bool is_gl_loaded_ = false;

    window_cursor_move_t last_cursor_;
    boost::signals2::scoped_connection mouse_grab_conn_;
  };

} // namespace chunklands::engine

#endif