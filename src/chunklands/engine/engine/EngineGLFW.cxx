
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine/shared.hxx>

namespace chunklands::engine {

AsyncEngineResult<CENone>
Engine::GLFWInit()
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this]() -> EngineResultX<CENone> {
        EASY_FUNCTION();
        const int result = glfwInit();
        data_->glfw.initialized = result == GLFW_TRUE;
        ENGINE_CHECK(data_->glfw.initialized);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        ENGINE_CHECK(monitor != nullptr);

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        ENGINE_CHECK(mode != nullptr);

        data_->gameloop.render_refresh_rate = mode->refreshRate;
        return Ok();
    });
}

EngineResultX<CENone> Engine::GLFWStartPollEvents(bool poll)
{
    ENGINE_FN();
    ENGINE_CHECK(data_->glfw.initialized);

    data_->glfw.start_poll_events = poll;
    return Ok();
}

EngineResultX<bool> Engine::GLFWStartPollEvents() const
{
    ENGINE_FN();
    ENGINE_CHECK(data_->glfw.initialized);

    return Ok(data_->glfw.start_poll_events);
}

} // namespace chunklands::engine