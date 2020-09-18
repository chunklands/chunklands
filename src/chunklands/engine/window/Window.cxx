
#include "Window.hxx"
#include <chunklands/libcxx/ThreadGuard.hxx>
#include <iostream>

namespace chunklands::engine::window {

namespace detail {
    inline Window* Unwrap(GLFWwindow* w)
    {
        void* const ptr = glfwGetWindowUserPointer(w);
        assert(ptr);

        return reinterpret_cast<Window*>(ptr);
    }
}

Window::Window(GLFWwindow* glfw_window)
    : glfw_window_(glfw_window)
{
    glfwSetWindowUserPointer(glfw_window, this);

    glfwSetWindowCloseCallback(glfw_window, [](GLFWwindow* w) {
        Window* const thiz = detail::Unwrap(w);
        thiz->on_close();
    });

    UpdateFramebufferSize();

    {
        size size;
        glfwGetWindowSize(glfw_window_, &size.width, &size.height);
        size_ = size; // atomic
    }

    glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow* w, int width, int height) {
        Window* const thiz = detail::Unwrap(w);
        size s { .width = width, .height = height };
        thiz->size_ = s;

        thiz->UpdateFramebufferSize();

        thiz->on_resize(s);
        thiz->on_content_resize(content_size {
            thiz->framebuffer_size_,
            thiz->content_scale_ });
    });

    {
        scale content_scale;
        glfwGetWindowContentScale(glfw_window_, &content_scale.x, &content_scale.y);
        content_scale_ = content_scale;
    }

    glfwSetWindowContentScaleCallback(glfw_window, [](GLFWwindow* w, float xscale, float yscale) {
        Window* const thiz = detail::Unwrap(w);

        scale s { .x = xscale, .y = yscale };
        thiz->content_scale_ = s;

        thiz->UpdateFramebufferSize();

        thiz->on_content_scale(s);
        thiz->on_content_resize(content_size {
            thiz->framebuffer_size_,
            thiz->content_scale_ });
    });

    glfwSetCursorPosCallback(glfw_window, [](GLFWwindow* w, double x, double y) {
        Window* const thiz = detail::Unwrap(w);
        thiz->on_cursor_move(cursor_move {
            .x = x,
            .y = y });
    });

    glfwSetMouseButtonCallback(glfw_window, [](GLFWwindow* w, int button, int action, int mods) {
        Window* const thiz = detail::Unwrap(w);
        thiz->on_click(click {
            .button = button,
            .action = action,
            .mods = mods });
    });

    glfwSetKeyCallback(glfw_window_, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        Window* const thiz = detail::Unwrap(w);
        thiz->on_key(window::key {
            .key = key,
            .scancode = scancode,
            .action = action,
            .mods = mods });
    });

    glfwSetScrollCallback(glfw_window, [](GLFWwindow* w, double x, double y) {
        Window* const thiz = detail::Unwrap(w);
        thiz->on_scroll(scroll {
            .x = x,
            .y = y });
    });
}

Window::~Window()
{
    assert(libcxx::ThreadGuard::IsMainThread());

    glfwDestroyWindow(glfw_window_);
    std::cout << "~Window" << std::endl;
}

void Window::UpdateFramebufferSize()
{
    size framebuffer_size;
    glfwGetFramebufferSize(glfw_window_, &framebuffer_size.width, &framebuffer_size.height);
    framebuffer_size_ = framebuffer_size;
}

bool Window::LoadGL()
{
    assert(libcxx::ThreadGuard::IsOpenGLThread());

    glfwMakeContextCurrent(glfw_window_);
    glfwSwapInterval(1);
    const int result = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
    is_gl_loaded_ = result != 0;
    return is_gl_loaded_;
}

void Window::SwapBuffers()
{
    assert(libcxx::ThreadGuard::IsOpenGLThread());
    glfwSwapBuffers(glfw_window_);
}

void Window::StartMouseGrab()
{
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(glfw_window_, &last_cursor_.x, &last_cursor_.y);

    // TODO(daaitch): can we add a handler for releasing mouse grab for debugging?
    // #include <csignal>
    // std::signal(SIGTRAP, [](int sig) {
    //   std::cout << "SIG: " << sig << std::endl;
    // });

    mouse_grab_conn_ = on_cursor_move.connect([this](const cursor_move& move) {
        mouse_grab event {
            .dx = last_cursor_.x - move.x,
            .dy = last_cursor_.y - move.y
        };

        last_cursor_ = std::move(move);

        on_mouse_grab(std::move(event));
    });
}

void Window::StopMouseGrab()
{
    glfwSetInputMode(glfw_window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouse_grab_conn_.disconnect();
}

} // namespace chunklands::engine::window