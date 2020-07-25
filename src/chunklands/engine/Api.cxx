
#include "Api.hxx"
#include <chunklands/libcxx/glfw.hxx>
#include "Window.hxx"
#include "engine_exception.hxx"

namespace chunklands::engine {

  void Api::RunCommands() {
    loop_.run_queued_closures();

    if (GLFW_start_poll_events) {
      glfwPollEvents();
    }
  }

  boost::future<bool>
  Api::GLFWInit() {
    return EnqueueTask([]() {
      const int result = glfwInit();
      return result == GLFW_TRUE;
    });
  }

  void
  Api::GLFWStartPollEvents(bool poll) {
    GLFW_start_poll_events = poll;
  }

  boost::future<bool>
  Api::GLFWLoadGL() {
    return EnqueueTask([]() {
      if (!glfwGetCurrentContext()) {
        engine::throw_engine_exception("GLFWLoadGL");
      }

      const int result = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
      return result != 0;
    });
  }

  boost::future<WindowHandle*>
  Api::WindowCreate(int width, int height, std::string title) {
    return EnqueueTask([width, height, title = std::move(title)]() {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      GLFWwindow* const glfw_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
      if (!glfw_window) {
        engine::throw_engine_exception("WindowCreate");
      }
      
      Window* const window = new Window(glfw_window);
      return reinterpret_cast<WindowHandle*>(window);
    });
  }

  void
  Api::WindowMakeContextCurrent(WindowHandle* handle) {
    Window* const window = reinterpret_cast<Window*>(handle);
    assert(window);

    EnqueueTask([window]() {
      window->makeContextCurrent();
    });
  }

  boost::signals2::scoped_connection
  Api::WindowOn(WindowHandle* handle, const std::string& event, std::function<void()> callback) {
    Window* const window = reinterpret_cast<Window*>(handle);
    assert(window);

    if (event == "shouldclose") {
      return window->on_close.connect(std::move(callback));
    }

    return boost::signals2::scoped_connection();
  }

} // namespace chunklands::engine