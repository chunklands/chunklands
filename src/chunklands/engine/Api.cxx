
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

  boost::future<void>
  Api::GLFWInit() {
    return EnqueueTask([]() {
      const int result = glfwInit();
      if (result != GLFW_TRUE) {
        throw_engine_exception("GLFWInit");
      }
    });
  }

  void
  Api::GLFWStartPollEvents(bool poll) {
    GLFW_start_poll_events = poll;
  }

  boost::future<WindowHandle*>
  Api::WindowCreate(int width, int height, std::string title) {
    return EnqueueTask([this, width, height, title = std::move(title)]() {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      GLFWwindow* const glfw_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
      if (!glfw_window) {
        throw_engine_exception("WindowCreate");
      }
      
      assert(glfw_window);
      Window* const window = new Window(glfw_window);
      WindowHandle* const window_handle = reinterpret_cast<WindowHandle*>(window);
      window_instances_.insert(window_handle);
      return window_handle;
    });
  }

  boost::future<void>
  Api::WindowLoadGL(WindowHandle* handle) {
    return EnqueueTask([handle]() {
      if (!handle) {
        throw_engine_exception("WindowLoadGL");
      }

      assert(handle);
      Window* const window = reinterpret_cast<Window*>(handle);
      assert(window);

      const bool gl_loaded = window->LoadGL();
      if (!gl_loaded) {
        throw_engine_exception("GLFWLoadGL");
      }
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