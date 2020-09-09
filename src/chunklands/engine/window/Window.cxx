
#include "Window.hxx"
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

    InitializeSize();
    glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow* w, int width, int height) {
        Window* const thiz = detail::Unwrap(w);
        size s { .width = width, .height = height };
        thiz->size_ = s;
        thiz->on_resize(s);
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
            .y = y
        });
    });
}

Window::~Window()
{
    glfwDestroyWindow(glfw_window_);
    std::cout << "~Window" << std::endl;
}

bool Window::LoadGL()
{
    glfwMakeContextCurrent(glfw_window_);
    glfwSwapInterval(1);
    const int result = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
    is_gl_loaded_ = result != 0;
    return is_gl_loaded_;
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(glfw_window_);
}

void Window::InitializeSize()
{
    size size;
    glfwGetWindowSize(glfw_window_, &size.width, &size.height);

    size_ = size;
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