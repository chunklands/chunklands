
#include "Window.hxx"

namespace chunklands::engine {

  namespace detail {
    inline Window* Unwrap(GLFWwindow* w) {
      void* const ptr = glfwGetWindowUserPointer(w);
      assert(ptr);

      return reinterpret_cast<Window*>(ptr);
    }
  }

  Window::Window(GLFWwindow* glfw_window): glfw_window_(glfw_window) {
    glfwSetWindowUserPointer(glfw_window, this);

    glfwSetWindowCloseCallback(glfw_window, [](GLFWwindow* w) {
      Window* const thiz = detail::Unwrap(w);
      thiz->on_close();
    });
  }

  Window::~Window() {
    glfwDestroyWindow(glfw_window_);
  }

  void Window::makeContextCurrent() {
    glfwMakeContextCurrent(glfw_window_);
  }

} // namespace chunklands::engine