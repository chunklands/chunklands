#ifndef __CHUNKLANDS_ENGINE_WINDOW_WINDOWINPUTCONTROLLER_HXX__
#define __CHUNKLANDS_ENGINE_WINDOW_WINDOWINPUTCONTROLLER_HXX__

#include "Window.hxx"

namespace chunklands::engine::window {

struct interaction_cursor {
    double dx = 0;
    double dy = 0;
};

struct interaction_move {
    double forward = 0;
    double left = 0;
    double right = 0;
    double back = 0;
};

struct interaction_delta {
    interaction_cursor cursor;
    interaction_move move;
    bool jump = false;
};

class WindowInputController {
public:
    WindowInputController(Window* window);

    interaction_delta GetAndResetInteraction(double now);

private:
    interaction_cursor cursor_;
    interaction_move move_;
    boost::signals2::scoped_connection window_on_cursor_move_conn_;
    boost::signals2::scoped_connection window_on_key_conn_;

    bool forward_pending_ = false;
    double forward_start_ = 0;
    bool left_pending_ = false;
    double left_start_ = 0;
    bool right_pending_ = false;
    double right_start_ = 0;
    bool back_pending_ = false;
    double back_start_ = 0;
    bool jump_ = false;
};

} // namespace chunklands::engine::window

#endif