#ifndef __CHUNKLANDS_ENGINE_WINDOW_HXX__
#define __CHUNKLANDS_ENGINE_WINDOW_HXX__

#include <chunklands/libcxx/glfw.hxx>
#include <boost/signals2.hpp>

namespace chunklands::engine {

  class Window {
  private:
    static inline Window* Unwrap(GLFWwindow* w) {
      void* const ptr = glfwGetWindowUserPointer(w);
      assert(ptr);

      return reinterpret_cast<Window*>(ptr);
    }

  public:
    Window(GLFWwindow* glfw_window): glfw_window_(glfw_window) {
      glfwSetWindowUserPointer(glfw_window, this);

      glfwSetWindowCloseCallback(glfw_window, [](GLFWwindow* w) {
        Window* const thiz = Unwrap(w);
        thiz->on_close();
      });
    }

    ~Window() {
      glfwDestroyWindow(glfw_window_);
    }

  public:
    boost::signals2::signal<void()> on_close;

  private:
    GLFWwindow* glfw_window_ = nullptr;
    
  };

} // namespace chunklands::engine

#endif