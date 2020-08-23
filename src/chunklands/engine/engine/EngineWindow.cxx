
#include <chunklands/engine/Window.hxx>
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine/shared.hxx>

namespace chunklands::engine {

AsyncEngineResult<CEWindowHandle*>
Engine::WindowCreate(int width, int height, std::string title)
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, width, height, title = std::move(title)]() -> EngineResultX<CEWindowHandle*> {
        EASY_FUNCTION();

        ENGINE_CHECKX(data_->glfw.initialized);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        GLFWwindow* const glfw_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
        ENGINE_CHECKX(glfw_window != nullptr);

        std::unique_ptr<Window> window = std::make_unique<Window>(glfw_window);
        data_->window.windows.insert(window.get());

        std::unique_ptr<WindowInputController> window_input_controller = std::make_unique<WindowInputController>(window.get());
        data_->window.window_input_controllers.insert({ window.get(), window_input_controller.get() });
        window_input_controller.release();

        CEWindowHandle* handle = reinterpret_cast<CEWindowHandle*>(window.get());
        window.release();
        return Ok(std::move(handle));
    });
}

AsyncEngineResult<CENone>
Engine::WindowLoadGL(CEWindowHandle* handle)
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle]() -> EngineResultX<CENone> {
        EASY_FUNCTION();

        ENGINE_CHECKX(has_handle(data_->window.windows, handle));
        Window* const window = reinterpret_cast<Window*>(handle);

        const bool gl_loaded = window->LoadGL();
        ENGINE_CHECKX(gl_loaded);

        return Ok();
    });
}

EventConnection
Engine::WindowOn(CEWindowHandle* handle, const std::string& event, std::function<void(CEWindowEvent)> callback)
{
    EASY_FUNCTION();
    ENGINE_FN();
    Window* const window = reinterpret_cast<Window*>(handle);
    CHECK_OR_FATAL(has_handle(data_->window.windows, window));
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