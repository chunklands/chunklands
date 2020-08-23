
#include "WindowInputController.hxx"

namespace chunklands::engine {

constexpr double factor = 0.002;

WindowInputController::WindowInputController(Window* window)
{
    window_on_cursor_move_conn = window->on_mouse_grab.connect([this](const window_mouse_grab_t& grab) {
        cursor_.dx += grab.dx * factor;
        cursor_.dy += grab.dy * factor;
    });

    window_on_key_conn = window->on_key.connect([this](const window_key_t& key) {
        if (key.key == GLFW_KEY_W) {
            if (key.action == GLFW_PRESS) {
                move_action_.forward_pending = true;
                move_action_.forward_start = glfwGetTime();
            } else if (key.action == GLFW_RELEASE) {
                move_.forward += glfwGetTime() - move_action_.forward_start;
                move_action_.forward_pending = false;
            }
        } else if (key.key == GLFW_KEY_S) {
            if (key.action == GLFW_PRESS) {
                move_action_.back_pending = true;
                move_action_.back_start = glfwGetTime();
            } else if (key.action == GLFW_RELEASE) {
                move_.back += glfwGetTime() - move_action_.back_start;
                move_action_.back_pending = false;
            }
        } else if (key.key == GLFW_KEY_A) {
            if (key.action == GLFW_PRESS) {
                move_action_.left_pending = true;
                move_action_.left_start = glfwGetTime();
            } else if (key.action == GLFW_RELEASE) {
                move_.left += glfwGetTime() - move_action_.left_start;
                move_action_.left_pending = false;
            }
        } else if (key.key == GLFW_KEY_D) {
            if (key.action == GLFW_PRESS) {
                move_action_.right_pending = true;
                move_action_.right_start = glfwGetTime();
            } else if (key.action == GLFW_RELEASE) {
                move_.right += glfwGetTime() - move_action_.right_start;
                move_action_.right_pending = false;
            }
        }
    });
}

windowinputcontroller_cursor_t WindowInputController::GetAndResetCursorDelta()
{
    const windowinputcontroller_cursor_t result = cursor_; // copy
    cursor_ = windowinputcontroller_cursor_t();
    return result;
}

windowinputcontroller_move_t WindowInputController::GetAndResetMoveDelta()
{
    const double now = glfwGetTime();

    if (move_action_.forward_pending) {
        move_.forward += now - move_action_.forward_start;
        move_action_.forward_start = now;
    }

    if (move_action_.back_pending) {
        move_.back += now - move_action_.back_start;
        move_action_.back_start = now;
    }

    if (move_action_.left_pending) {
        move_.left += now - move_action_.left_start;
        move_action_.left_start = now;
    }

    if (move_action_.right_pending) {
        move_.right += now - move_action_.right_start;
        move_action_.right_start = now;
    }

    const windowinputcontroller_move_t result = move_; // copy
    move_ = windowinputcontroller_move_t();
    return result;
}

} // namespace chunklands::engine