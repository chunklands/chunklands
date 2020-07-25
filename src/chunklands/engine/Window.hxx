#ifndef __CHUNKLANDS_ENGINE_WINDOW_HXX__
#define __CHUNKLANDS_ENGINE_WINDOW_HXX__

#include <chunklands/libcxx/glfw.hxx>
#include <boost/signals2.hpp>

namespace chunklands::engine {

  class Window {
  public:
    Window(GLFWwindow* glfw_window);
    ~Window();

    bool LoadGL();

  public:
    boost::signals2::signal<void()> on_close;

  private:
    GLFWwindow* glfw_window_ = nullptr;
    bool is_gl_loaded_ = false;
  };

} // namespace chunklands::engine

#endif