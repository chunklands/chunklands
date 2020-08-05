
#include "api-shared.hxx"

namespace chunklands::engine {

  boost::future<void>
  Api::GLFWInit() {
    EASY_FUNCTION();
    API_FN();

    return EnqueueTask(executor_, [this]() {
      EASY_FUNCTION();
      const int result = glfwInit();
      GLFW_initialized = result == GLFW_TRUE;
      CHECK(GLFW_initialized);
    });
  }

  void
  Api::GLFWStartPollEvents(bool poll) {
    API_FN();
    CHECK(GLFW_initialized);

    GLFW_start_poll_events_ = poll;
  }

} // namespace chunklands::engine