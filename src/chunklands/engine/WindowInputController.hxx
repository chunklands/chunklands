#ifndef __CHUNKLANDS_WINDOWINPUTCONTROLLER_HXX__
#define __CHUNKLANDS_WINDOWINPUTCONTROLLER_HXX__

#include "Window.hxx"

namespace chunklands::engine {

struct windowinputcontroller_cursor_t {
    double dx = 0;
    double dy = 0;
};

struct windowinputcontroller_move_t {
    double forward = 0;
    double left = 0;
    double right = 0;
    double back = 0;
};

struct windowinputcontroller_moveaction_t {
    bool forward_pending = false;
    double forward_start = 0;
    bool left_pending = false;
    double left_start = 0;
    bool right_pending = false;
    double right_start = 0;
    bool back_pending = false;
    double back_start = 0;
};

class WindowInputController {
public:
    WindowInputController(Window* window);

    windowinputcontroller_cursor_t GetAndResetCursorDelta();
    windowinputcontroller_move_t GetAndResetMoveDelta();

private:
    windowinputcontroller_cursor_t cursor_;
    windowinputcontroller_move_t move_;
    windowinputcontroller_moveaction_t move_action_;
    boost::signals2::scoped_connection window_on_cursor_move_conn;
    boost::signals2::scoped_connection window_on_key_conn;
};

} // namespace chunklands::engine

#endif