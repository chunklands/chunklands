
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
      data_->windows.insert(window.get());

      std::unique_ptr<WindowInputController> window_input_controller = std::make_unique<WindowInputController>(window.get());
      data_->window_input_controllers.insert({window.get(), window_input_controller.get()});
      window_input_controller.release();
      
      CEWindowHandle* const handle = reinterpret_cast<CEWindowHandle*>(window.get());
      window.release();
      return handle;
    });
  }

  boost::future<void>
  Api::WindowLoadGL(CEWindowHandle* handle) {
    EASY_FUNCTION();
    API_FN();
    Window* const window = reinterpret_cast<Window*>(handle);
    CHECK(has_handle(data_->windows, window));

    return EnqueueTask(executor_, [window]() {
      EASY_FUNCTION();
      assert(window);

      const bool gl_loaded = window->LoadGL();
      CHECK(gl_loaded);
    });
  }

  boost::signals2::scoped_connection
  Api::WindowOn(CEWindowHandle* handle, const std::string& event, std::function<void(CEWindowEvent)> callback) {
    EASY_FUNCTION();
    API_FN();
    Window* const window = reinterpret_cast<Window*>(handle);
    CHECK(has_handle(data_->windows, window));
    assert(window);

    if (event == "shouldclose") {
      return window->on_close.connect([callback = std::move(callback)]() {
        callback(CEWindowEvent("shouldclose"));
      });
    }

    if (event == "click") {
      return window->on_click.connect([callback = std::move(callback)](const window_click_t& click) {
        CEWindowEvent event("click");
        event.click.button = click.button;
        event.click.action = click.action;
        event.click.mods = click.mods;
        callback(std::move(event));
      });
    }

    if (event == "key") {
      return window->on_key.connect([callback = std::move(callback)](const window_key_t& key) {
        CEWindowEvent event("key");
        event.key.key = key.key;
        event.key.scancode = key.scancode;
        event.key.action = key.action;
        event.key.mods = key.mods;
        callback(std::move(event));
      });
    }

    return boost::signals2::scoped_connection();
  }

} // namespace chunklands::engine