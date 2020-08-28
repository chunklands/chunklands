
#include "CharacterController.hxx"
#include <cmath>

namespace chunklands::engine::character {

constexpr float LOOK_RADIANS = M_PI * 0.9;
constexpr float MIN_PITCH = -LOOK_RADIANS / 2.0;
constexpr float MAX_PITCH = +LOOK_RADIANS / 2.0;

void CharacterController::MoveAndLook(const glm::vec2& forward_right, const glm::vec2& look_delta)
{
    {
        const float length = glm::length(forward_right);
        float forward = forward_right[0];
        float right = forward_right[1];

        if (length > 1.f) {
            forward /= length;
            right /= length;
        }

        // TODO(daaitch): collision here
        const glm::vec2& look = camera_.GetLook();
        const float yaw = look.x;
        const float pitch = look.y;
        const float sin_yaw = std::sin(yaw);
        const float cos_yaw = std::cos(yaw);
        const float sin_pitch = std::sin(pitch);
        const float cos_pitch = std::cos(pitch);

        glm::vec3 move(
            cos_pitch * sin_yaw * forward + -cos_yaw * right,
            sin_pitch * forward,
            cos_pitch * cos_yaw * forward + sin_yaw * right);

        auto response = movement_controller_.CalculateMovement(engine_chunk_data_, camera_.GetEye(), std::move(move));
        camera_.SetEye(response.new_camera_pos);
    }

    {
        glm::vec2 look = camera_.GetLook() + look_delta;

        // yaw modulo
        look.x = std::fmod(look.x, float(2.f * M_PI));

        // pitch range
        look.y = std::max(MIN_PITCH, look.y);
        look.y = std::min(MAX_PITCH, look.y);

        camera_.SetLook(look);
    }

    engine_render_data_.pointing_block = movement_controller_.PointingBlock(engine_chunk_data_, math::fLine3(camera_.GetEye(), camera_.GetViewDirection() * 2.f));
}

} // namespace chunklands::engine::character