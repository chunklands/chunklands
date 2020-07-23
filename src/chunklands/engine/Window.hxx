#ifndef __CHUNKLANDS_ENGINE_WINDOW_HXX__
#define __CHUNKLANDS_ENGINE_WINDOW_HXX__

#include <chunklands/libcxx/glfw.hxx>
#include <boost/signals2.hpp>

namespace chunklands::engine {

  class Window {
  public:
    Window(GLFWwindow* glfw_window);
    ~Window();

    void makeContextCurrent();

  public:
    boost::signals2::signal<void()> on_close;

  private:
    GLFWwindow* glfw_window_ = nullptr;
    
  };

} // namespace chunklands::engine

#endif