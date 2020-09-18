
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"
#include <chunklands/libcxx/easy_profiler.hxx>

namespace chunklands::engine {

AsyncEngineResult<CENone>
Engine::GLFWInit()
{
    EASY_FUNCTION();
    ENGINE_FN();

    const int result = glfwInit();
    const bool initialized = result == GLFW_TRUE;
    ENGINE_CHECK(initialized);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    ENGINE_CHECK(monitor != nullptr);

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    ENGINE_CHECK(mode != nullptr);

    const int refresh_rate = mode->refreshRate;

    return EnqueueTask(data_->executors.opengl, [this, initialized, refresh_rate]() -> EngineResultX<CENone> {
        EASY_FUNCTION();

        assert(initialized);
        data_->glfw.initialized = initialized;
        data_->gameloop.render_refresh_rate = refresh_rate;

        return Ok();
    });
}

EngineResultX<CENone>
Engine::GLFWPollEvents()
{
    glfwPollEvents();
    return Ok();
}

} // namespace chunklands::engine