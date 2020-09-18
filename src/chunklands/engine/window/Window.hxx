#ifndef __CHUNKLANDS_ENGINE_WINDOW_WINDOW_HXX__
#define __CHUNKLANDS_ENGINE_WINDOW_WINDOW_HXX__

#include <boost/atomic.hpp>
#include <boost/signals2.hpp>
#include <chunklands/libcxx/glfw.hxx>

namespace chunklands::engine::window {

struct size {
    int width = 0;
    int height = 0;
};

struct scale {
    float x = 0.f;
    float y = 0.f;
};

struct content_size {
    size size;
    scale scale;
};

struct cursor_move {
    double x = 0;
    double y = 0;
};

struct mouse_grab {
    double dx = 0;
    double dy = 0;
};

struct click {
    int button = 0;
    int action = 0;
    int mods = 0;
};

struct key {
    int key = 0;
    int scancode = 0;
    int action = 0;
    int mods = 0;
};

struct scroll {
    double x = 0.0;
    double y = 0.0;
};

template <class T>
struct slot_t {
    using value = boost::signals2::signal<void(const T&)>;
};

template <>
struct slot_t<void> {
    using value = boost::signals2::signal<void()>;
};

template <class T>
using slot = typename slot_t<T>::value;

class Window {
public:
    Window(GLFWwindow* glfw_window);
    ~Window();

    bool LoadGL();
    void SwapBuffers();
    bool IsGLLoaded() const
    {
        return is_gl_loaded_;
    }

    size GetSize() const
    {
        return size_;
    }

    scale GetContentScale() const
    {
        return content_scale_;
    }

    size GetFramebufferSize() const
    {
        return framebuffer_size_;
    }

    content_size GetContentSize() const
    {
        return {
            framebuffer_size_,
            content_scale_
        };
    }

    void StartMouseGrab();
    void StopMouseGrab();

private:
    void UpdateFramebufferSize();

public:
    slot<void> on_close;
    slot<size> on_resize;
    slot<cursor_move> on_cursor_move;
    slot<click> on_click;
    slot<mouse_grab> on_mouse_grab;
    slot<key> on_key;
    slot<scroll> on_scroll;
    slot<scale> on_content_scale;
    slot<content_size> on_content_resize;

private:
    GLFWwindow* glfw_window_ = nullptr;
    bool is_gl_loaded_ = false;

    cursor_move last_cursor_;
    boost::signals2::scoped_connection mouse_grab_conn_;
    boost::atomic<size> size_;
    boost::atomic<scale> content_scale_;
    boost::atomic<size> framebuffer_size_;
};

} // namespace chunklands::engine::window

#endif