
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"
#include <chunklands/engine/window/Window.hxx>
#include <chunklands/libcxx/ThreadGuard.hxx>
#include <chunklands/libcxx/easy_profiler.hxx>

namespace chunklands::engine {

AsyncEngineResult<CEWindowHandle*>
Engine::WindowCreate(int width, int height, std::string title)
{
    EASY_FUNCTION();
    ENGINE_FN();

    ENGINE_CHECK(data_->glfw.initialized);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    GLFWwindow* const glfw_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    ENGINE_CHECK(glfw_window != nullptr);

    std::unique_ptr<window::Window> window = std::make_unique<window::Window>(glfw_window);

    return EnqueueTask(data_->executors.opengl, [this, window = std::move(window)]() mutable -> EngineResultX<CEWindowHandle*> {
        EASY_FUNCTION();

        data_->window.windows.insert(window.get());

        std::unique_ptr<window::WindowInputController> window_input_controller = std::make_unique<window::WindowInputController>(window.get());
        data_->window.window_input_controllers.insert({ window.get(), window_input_controller.get() });
        window_input_controller.release();

        return Ok(reinterpret_cast<CEWindowHandle*>(window.release()));
    });
}

AsyncEngineResult<CENone>
Engine::WindowLoadGL(CEWindowHandle* handle)
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle]() -> EngineResultX<CENone> {
        EASY_FUNCTION();

        window::Window* window = nullptr;
        ENGINE_CHECK(get_handle(&window, data_->window.windows, handle));

        const bool gl_loaded = window->LoadGL();
        ENGINE_CHECK(gl_loaded);

        return Ok();
    });
}

EngineResultX<EventConnection>
Engine::WindowOn(CEWindowHandle* handle, const std::string& event, std::function<void(CEWindowEvent)> main_thread_callback)
{
    EASY_FUNCTION();
    ENGINE_FN();

    window::Window* window = nullptr;
    ENGINE_CHECK(get_handle(&window, data_->window.windows, handle));

    if (event == "shouldclose") {
        return Ok(EventConnection(window->on_close.connect([main_thread_callback = std::move(main_thread_callback)]() {
            assert(libcxx::ThreadGuard::IsMainThread());

            main_thread_callback(CEWindowEvent("shouldclose"));
        })));
    }

    if (event == "click") {
        return Ok(EventConnection(window->on_click.connect([main_thread_callback = std::move(main_thread_callback)](const window::click& click) {
            assert(libcxx::ThreadGuard::IsMainThread());

            CEWindowEvent event("click");
            event.click.button = click.button;
            event.click.action = click.action;
            event.click.mods = click.mods;
            main_thread_callback(std::move(event));
        })));
    }

    if (event == "key") {
        return Ok(EventConnection(window->on_key.connect([main_thread_callback = std::move(main_thread_callback)](const window::key& key) {
            assert(libcxx::ThreadGuard::IsMainThread());

            CEWindowEvent event("key");
            event.key.key = key.key;
            event.key.scancode = key.scancode;
            event.key.action = key.action;
            event.key.mods = key.mods;
            main_thread_callback(std::move(event));
        })));
    }

    if (event == "resize") {
        return Ok(EventConnection(window->on_resize.connect([main_thread_callback = std::move(main_thread_callback)](const window::size& size) {
            assert(libcxx::ThreadGuard::IsMainThread());

            CEWindowEvent event("resize");
            event.resize.width = size.width;
            event.resize.height = size.height;
            main_thread_callback(std::move(event));
        })));
    }

    if (event == "contentresize") {
        return Ok(EventConnection(window->on_content_resize.connect([main_thread_callback = std::move(main_thread_callback)](const window::content_size& content_size) {
            assert(libcxx::ThreadGuard::IsMainThread());

            CEWindowEvent event("contentresize");
            event.content_resize.width = content_size.size.width;
            event.content_resize.height = content_size.size.height;
            event.content_resize.xscale = content_size.scale.x;
            event.content_resize.yscale = content_size.scale.y;
            main_thread_callback(std::move(event));
        })));
    }

    if (event == "scroll") {
        return Ok(EventConnection(window->on_scroll.connect([main_thread_callback = std::move(main_thread_callback)](const window::scroll& scroll) {
            assert(libcxx::ThreadGuard::IsMainThread());

            CEWindowEvent event("scroll");
            event.scroll.x = scroll.x;
            event.scroll.y = scroll.y;
            main_thread_callback(std::move(event));
        })));
    }

    return Ok();
}

EngineResultX<CESize2i> Engine::WindowGetSize(CEWindowHandle* handle)
{
    ENGINE_FN();

    window::Window* window = nullptr;
    ENGINE_CHECK(get_handle(&window, data_->window.windows, handle));

    window::size size = window->GetSize();
    return Ok(CESize2i { .width = size.width, .height = size.height });
}

EngineResultX<CEWindowContentSize> Engine::WindowGetContentSize(CEWindowHandle* handle)
{
    ENGINE_FN();

    window::Window* window = nullptr;
    ENGINE_CHECK(get_handle(&window, data_->window.windows, handle));

    window::content_size content_size = window->GetContentSize();
    return Ok(CEWindowContentSize {
        .width = content_size.size.width,
        .height = content_size.size.height,
        .xscale = content_size.scale.x,
        .yscale = content_size.scale.y });
}

} // namespace chunklands::engine