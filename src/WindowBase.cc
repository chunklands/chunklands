#include "WindowBase.h"

namespace chunklands {

  namespace detail {
    WindowBase* Unwrap(GLFWwindow* window) {
      return static_cast<WindowBase*>(glfwGetWindowUserPointer(window));
    }
  }
  
  JS_DEF_WRAP(WindowBase)

  void WindowBase::JSCall_initialize(JSCbi info) {
    auto&& env = info.Env();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    auto&& config = info[0].ToObject();
    window_ = glfwCreateWindow(
      config.Get("width").ToNumber(),
      config.Get("height").ToNumber(),
      config.Get("title").ToString().Utf8Value().c_str(),
      nullptr,
      nullptr
    );

    JS_ASSERT(window_ != nullptr);

    glfwSetWindowUserPointer(window_, this);

    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
      detail::Unwrap(window)->on_resize(width, height);
    });

    glfwSetInputMode(window_, GLFW_STICKY_KEYS, 1);
    glfwShowWindow(window_);
  }

  void WindowBase::JSCall_makeContextCurrent(JSCbi) {
    glfwMakeContextCurrent(window_);
  }

  JSValue WindowBase::JSCall_shouldClose(JSCbi info) {
    if (!window_) {
      return Napi::Boolean::New(info.Env(), true);
    }

    const int should_close = glfwWindowShouldClose(window_);
    return Napi::Boolean::New(info.Env(), should_close == GLFW_TRUE);
  }

  void WindowBase::JSCall_close(JSCbi) {
    if (!window_) {
      return;
    }

    glfwDestroyWindow(window_);
    window_ = nullptr;
  }

  void WindowBase::SwapBuffers() {
    if (!window_) {
      return;
    }
    
    glfwSwapBuffers(window_);
  }

  int WindowBase::GetKey(int key) {
    return glfwGetKey(window_, key);
  }

  int WindowBase::GetMouseButton(int button) {
    return glfwGetMouseButton(window_, button);
  }

  glm::dvec2 WindowBase::GetCursorPos() const {
    glm::dvec2 pos;
    glfwGetCursorPos(window_, &pos.x, &pos.y);
    return pos;
  }

  glm::ivec2 WindowBase::GetSize() const {
    glm::ivec2 size;
    glfwGetWindowSize(window_, &size.x, &size.y);
    return size;
  }

  void WindowBase::StartMouseGrab() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double xpos, double ypos) {
      detail::Unwrap(window)->on_cursor_move(xpos, ypos);
    });
  }

  void WindowBase::StopMouseGrab() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window_, nullptr);
  }
}