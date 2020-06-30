

#include "Window.hxx"

namespace chunklands::engine {

  namespace detail {
    Window* Unwrap(GLFWwindow* window) {
      return static_cast<Window*>(glfwGetWindowUserPointer(window));
    }
  }
  
  JS_DEF_WRAP(Window)

  void Window::JSCall_initialize(JSCbi info) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    auto&& config = info[0].ToObject();
    window_ = glfwCreateWindow(
      config.Get("width").ToNumber(),
      config.Get("height").ToNumber(),
      config.Get("title").ToString().Utf8Value().c_str(),
      nullptr,
      nullptr
    );

    JS_ASSERT(info.Env(), window_ != nullptr);

    glfwSetWindowUserPointer(window_, this);

    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
      detail::Unwrap(window)->on_resize(width, height);
    });

    events_ = Napi::Persistent(Value().Get("events").ToObject());

    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
      auto&& events = detail::Unwrap(window)->events_;
      auto&& env = events.Env();

      JSHandleScope scope(env);
      {
        auto&& event = JSObject::New(env);
        event["key"] = JSNumber::New(env, key);
        event["scancode"] = JSNumber::New(env, scancode);
        event["action"] = JSNumber::New(env, action);
        event["mods"] = JSNumber::New(env, mods);

        events.Get("emit").As<JSFunction>().Call(events.Value(), {JSString::New(events.Env(), "key"), event});
      }
    });

    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
      auto&& events = detail::Unwrap(window)->events_;
      auto&& env = events.Env();

      JSHandleScope scope(env);
      {
        auto&& event = JSObject::New(env);
        event["button"] = JSNumber::New(env, button);
        event["action"] = JSNumber::New(env, action);
        event["mods"] = JSNumber::New(env, mods);

        events.Get("emit").As<JSFunction>().Call(events.Value(), {JSString::New(events.Env(), "mousebutton"), event});
      }
    });

    // no sticky keys: we get strange behavior of pressing e.g. "F" emits: F down, F up, F up
    // glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwShowWindow(window_);

    glfwSwapInterval(1);
  }

  void Window::JSCall_makeContextCurrent(JSCbi) {
    glfwMakeContextCurrent(window_);
  }

  JSValue Window::JSCall_shouldClose(JSCbi info) {
    if (!window_) {
      return JSBoolean::New(info.Env(), true);
    }

    const int should_close = glfwWindowShouldClose(window_);
    return JSBoolean::New(info.Env(), should_close == GLFW_TRUE);
  }

  void Window::JSCall_close(JSCbi) {
    if (!window_) {
      return;
    }

    glfwDestroyWindow(window_);
    window_ = nullptr;
  }

  void Window::SwapBuffers() {
    if (!window_) {
      return;
    }
    
    glfwSwapBuffers(window_);
  }

  int Window::GetKey(int key) {
    return glfwGetKey(window_, key);
  }

  int Window::GetMouseButton(int button) {
    return glfwGetMouseButton(window_, button);
  }

  glm::dvec2 Window::GetCursorPos() const {
    glm::dvec2 pos;
    glfwGetCursorPos(window_, &pos.x, &pos.y);
    return pos;
  }

  glm::ivec2 Window::GetSize() const {
    glm::ivec2 size;
    glfwGetWindowSize(window_, &size.x, &size.y);
    return size;
  }

  void Window::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto&& me = detail::Unwrap(window);

    glm::dvec2 pos(xpos, ypos);
    glm::dvec2 cursor_diff = me->game_control_last_cursor_pos_ - pos;
    me->game_control_last_cursor_pos_ = pos;
    me->on_game_control_look(cursor_diff.x, cursor_diff.y);
  }

  void Window::StartMouseGrab() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, CursorPosCallback);
    game_control_last_cursor_pos_ = GetCursorPos();
  }

  void Window::StopMouseGrab() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window_, nullptr);
  }

} // namespace chunklands::engine