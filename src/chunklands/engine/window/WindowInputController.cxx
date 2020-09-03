
#include "WindowInputController.hxx"

namespace chunklands::engine::window {

constexpr double factor = 0.002;

WindowInputController::WindowInputController(Window* window)
{
    window_on_cursor_move_conn_ = window->on_mouse_grab.connect([this](const mouse_grab& grab) {
        cursor_.dx += grab.dx * factor;
        cursor_.dy += grab.dy * factor;
    });

    window_on_key_conn_ = window->on_key.connect([this](const key& key) {
        const double now = glfwGetTime();

        if (key.key == GLFW_KEY_W) {
            if (key.action == GLFW_PRESS) {
                forward_pending_ = true;
                forward_start_ = now;
            } else if (key.action == GLFW_RELEASE) {
                move_.forward += now - forward_start_;
                forward_pending_ = false;
            }
        } else if (key.key == GLFW_KEY_S) {
            if (key.action == GLFW_PRESS) {
                back_pending_ = true;
                back_start_ = now;
            } else if (key.action == GLFW_RELEASE) {
                move_.back += now - back_start_;
                back_pending_ = false;
            }
        } else if (key.key == GLFW_KEY_A) {
            if (key.action == GLFW_PRESS) {
                left_pending_ = true;
                left_start_ = now;
            } else if (key.action == GLFW_RELEASE) {
                move_.left += now - left_start_;
                left_pending_ = false;
            }
        } else if (key.key == GLFW_KEY_D) {
            if (key.action == GLFW_PRESS) {
                right_pending_ = true;
                right_start_ = now;
            } else if (key.action == GLFW_RELEASE) {
                move_.right += now - right_start_;
                right_pending_ = false;
            }
        } else if (key.key == GLFW_KEY_SPACE) {
            if (key.action == GLFW_PRESS) {
                jump_ = true;
            }
        }
    });
}

interaction_delta WindowInputController::GetAndResetInteraction(double now)
{
    // cursor
    interaction_delta interaction;
    interaction.cursor = cursor_;
    cursor_ = interaction_cursor();

    // move
    if (forward_pending_) {
        move_.forward += now - forward_start_;
        forward_start_ = now;
    }

    if (back_pending_) {
        move_.back += now - back_start_;
        back_start_ = now;
    }

    if (left_pending_) {
        move_.left += now - left_start_;
        left_start_ = now;
    }

    if (right_pending_) {
        move_.right += now - right_start_;
        right_start_ = now;
    }

    interaction.move = move_;
    move_ = interaction_move();

    // jump
    interaction.jump = jump_;
    jump_ = false;

    return interaction;
}

} // namespace chunklands::engine::window