
#include "api-shared.hxx"
#include "../Window.hxx"

namespace chunklands::engine {

  boost::future<CEWindowHandle*>
  Api::WindowCreate(int width, int height, std::string title) {
    EASY_FUNCTION();
    API_FN();
    CHECK(GLFW_initialized);

    return EnqueueTask(executor_, [this, width, height, title = std::move(title)]() {
      EASY_FUNCTION();

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      GLFWwindow* const glfw_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
      CHECK(glfw_window != nullptr);
      
      std::unique_ptr<Window> window = std::make_unique<Window>(glfw_window);
      CEWindowHandle* const handle = reinterpret_cast<CEWindowHandle*>(window.get());
      windows_.insert(handle);

      window.release();
      return handle;
    });
  }

  boost::future<void>
  Api::WindowLoadGL(CEWindowHandle* handle) {
    EASY_FUNCTION();
    API_FN();
    CHECK(has_handle(windows_, handle));

    return EnqueueTask(executor_, [handle]() {
      EASY_FUNCTION();
      Window* const window = reinterpret_cast<Window*>(handle);
      assert(window);

      const bool gl_loaded = window->LoadGL();
      CHECK(gl_loaded);
    });
  }

  boost::signals2::scoped_connection
  Api::WindowOn(CEWindowHandle* handle, const std::string& event, std::function<void()> callback) {
    EASY_FUNCTION();
    API_FN();
    CHECK(has_handle(windows_, handle));

    Window* const window = reinterpret_cast<Window*>(handle);
    assert(window);

    if (event == "shouldclose") {
      return window->on_close.connect(std::move(callback));
    }

    return boost::signals2::scoped_connection();
  }

} // namespace chunklands::engine