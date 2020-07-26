
#include "Window.hxx"
#include <iostream>

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

    glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow* w, int width, int height) {
      Window* const thiz = detail::Unwrap(w);
      thiz->on_resize(window_size_t {
        .width = width,
        .height = height
      });
    });
  }

  Window::~Window() {
    glfwDestroyWindow(glfw_window_);
    std::cout << "~Window" << std::endl;
  }

  bool Window::LoadGL() {
    glfwMakeContextCurrent(glfw_window_);
    const int result = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
    is_gl_loaded_ = result != 0;
    return is_gl_loaded_;
  }

  void Window::SwapBuffers() {
    glfwSwapBuffers(glfw_window_);
  }

  window_size_t Window::GetSize() const {
    window_size_t size;
    glfwGetWindowSize(glfw_window_, &size.width, &size.height);
    return size;
  }

} // namespace chunklands::engine