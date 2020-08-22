
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine/shared.hxx>

namespace chunklands::engine {

  boost::future<void>
  Engine::GLFWInit() {
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this]() {
      EASY_FUNCTION();
      const int result = glfwInit();
      data_->glfw.initialized = result == GLFW_TRUE;
      CHECK_OR_FATAL(data_->glfw.initialized);

      GLFWmonitor* monitor = glfwGetPrimaryMonitor();
      CHECK_OR_FATAL(monitor != nullptr);

      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      CHECK_OR_FATAL(mode != nullptr);

      data_->gameloop.render_refresh_rate = mode->refreshRate;
    });
  }

  void
  Engine::GLFWStartPollEvents(bool poll) {
    ENGINE_FN();
    CHECK_OR_FATAL(data_->glfw.initialized);

    data_->glfw.start_poll_events = poll;
  }

  bool
  Engine::GLFWStartPollEvents() const {
    ENGINE_FN();
    CHECK_OR_FATAL(data_->glfw.initialized);

    return data_->glfw.start_poll_events;
  }

} // namespace chunklands::engine