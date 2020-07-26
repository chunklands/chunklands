
#include "Api.hxx"
#include <chunklands/libcxx/glfw.hxx>
#include "Window.hxx"
#include "engine_exception.hxx"
#include <iostream>

namespace chunklands::engine {

  Api::~Api() {
    std::cout << "~Api" << std::endl;
  }

  void Api::Tick() {
    // TODO(daaitch): unnecessary glClear should be removed when render pipeline in place
    for (const WindowHandle* const handle : window_handles_) {
      const Window* const window = reinterpret_cast<const Window*>(handle);
      assert(window);

      if (window->IsGLLoaded()) {
        glClearColor(0, 0, 0, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }
    }

    if (GLFW_start_poll_events_) {
      glfwPollEvents();
    }

    for (WindowHandle* const handle : window_handles_) {
      Window* const window = reinterpret_cast<Window*>(handle);
      assert(window);

      if (window->IsGLLoaded()) {
        window->SwapBuffers();
      }
    }
  }

  boost::future<void>
  Api::GLFWInit() {
    return EnqueueTask([]() {
      const int result = glfwInit();
      if (result != GLFW_TRUE) {
        throw_engine_exception("GLFWInit");
      }
    });
  }

  void
  Api::GLFWStartPollEvents(bool poll) {
    GLFW_start_poll_events_ = poll;
  }

  boost::future<WindowHandle*>
  Api::WindowCreate(int width, int height, std::string title) {
    return EnqueueTask([this, width, height, title = std::move(title)]() {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      GLFWwindow* const glfw_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
      if (!glfw_window) {
        throw_engine_exception("WindowCreate");
      }
      
      assert(glfw_window);
      Window* const window = new Window(glfw_window);
      WindowHandle* const window_handle = reinterpret_cast<WindowHandle*>(window);
      window_handles_.insert(window_handle);
      return window_handle;
    });
  }

  boost::future<void>
  Api::WindowLoadGL(WindowHandle* handle) {
    return EnqueueTask([handle]() {
      if (!handle) {
        throw_engine_exception("WindowLoadGL");
      }

      assert(handle);
      Window* const window = reinterpret_cast<Window*>(handle);
      assert(window);

      const bool gl_loaded = window->LoadGL();
      if (!gl_loaded) {
        throw_engine_exception("GLFWLoadGL");
      }
    });
  }

  boost::signals2::scoped_connection
  Api::WindowOn(WindowHandle* handle, const std::string& event, std::function<void()> callback) {
    Window* const window = reinterpret_cast<Window*>(handle);
    assert(window);

    if (event == "shouldclose") {
      return window->on_close.connect(std::move(callback));
    }

    return boost::signals2::scoped_connection();
  }

  boost::future<void>
  Api::GBufferPassInit(WindowHandle* handle, std::string vertex_shader, std::string fragment_shader) {
    return EnqueueTask([this,
      handle,
      vertex_shader = std::move(vertex_shader),
      fragment_shader = std::move(fragment_shader)
    ]() {
      Window* const window = reinterpret_cast<Window*>(handle);
      assert(window);

      std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(vertex_shader.data(), fragment_shader.data());
      g_buffer_pass_ = std::make_unique<GBufferPass>(window, std::move(program));
    });
  }

} // namespace chunklands::engine