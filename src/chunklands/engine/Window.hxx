#ifndef __CHUNKLANDS_ENGINE_WINDOW_HXX__
#define __CHUNKLANDS_ENGINE_WINDOW_HXX__

#include <chunklands/libcxx/glfw.hxx>
#include <boost/signals2.hpp>

namespace chunklands::engine {

  struct window_size_t {
    int width;
    int height;
  };

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

  public:
    boost::signals2::signal<void()> on_close;
    boost::signals2::signal<void(const window_size_t& size)> on_resize;

  private:
    GLFWwindow* glfw_window_ = nullptr;
    bool is_gl_loaded_ = false;
  };

} // namespace chunklands::engine

#endif