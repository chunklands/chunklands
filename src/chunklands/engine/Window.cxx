
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

    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow* w, double x, double y) {
      Window* const thiz = detail::Unwrap(w);
      thiz->on_cursor_move(window_cursor_move_t {
        .x = x,
        .y = y
      });
    });

    glfwSetMouseButtonCallback(glfw_window, [](GLFWwindow* w, int button, int action, int mods) {
      Window* const thiz = detail::Unwrap(w);
      thiz->on_click(window_click_t {
        .button = button,
        .action = action,
        .mods = mods
      });
    });

    glfwSetKeyCallback(glfw_window_, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
      Window* const thiz = detail::Unwrap(w);
      thiz->on_key(window_key_t {
        .key = key,
        .scancode = scancode,
        .action = action,
        .mods = mods
      });
    });
  }

  Window::~Window() {
    glfwDestroyWindow(glfw_window_);
    std::cout << "~Window" << std::endl;
  }

  bool Window::LoadGL() {
    glfwMakeContextCurrent(glfw_window_);
    glfwSwapInterval(0);
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

  void Window::StartMouseGrab() {
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(glfw_window_, &last_cursor_.x, &last_cursor_.y);
    
    // TODO(daaitch): can we add a handler for releasing mouse grab for debugging?
    // #include <csignal>
    // std::signal(SIGTRAP, [](int sig) {
    //   std::cout << "SIG: " << sig << std::endl;
    // });

    mouse_grab_conn_ = on_cursor_move.connect([this](const window_cursor_move_t& move) {
      window_mouse_grab_t event {
        .dx = last_cursor_.x - move.x,
        .dy = last_cursor_.y - move.y
      };

      last_cursor_ = std::move(move);

      on_mouse_grab(std::move(event));
    });
  }

  void Window::StopMouseGrab() {
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouse_grab_conn_.disconnect();
  }

} // namespace chunklands::engine