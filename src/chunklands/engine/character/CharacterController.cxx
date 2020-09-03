
#include "CharacterController.hxx"
#include <chunklands/libcxx/easylogging++.hxx>
#include <cmath>

namespace chunklands::engine::character {

constexpr float LOOK_RADIANS = M_PI * 0.9;
constexpr float MIN_PITCH = -LOOK_RADIANS / 2.0;
constexpr float MAX_PITCH = +LOOK_RADIANS / 2.0;
constexpr float MOVE_SPEED = 7.5f;
constexpr float MOVE_SPEED_FLIGHT_MODE = 20.f;
// constexpr float GRAVITY = -9.81f;
constexpr float GRAVITY = -0.8f;

void CharacterController::MoveAndLook(double, double now, const glm::vec2& forward_right, const glm::vec2& look_delta, bool jump)
{
    // look
    {
        glm::vec2 look = camera_.GetLook() + look_delta;

        // yaw modulo
        look.x = std::fmod(look.x, float(2.f * M_PI));

        // pitch range
        look.y = std::max(MIN_PITCH, look.y);
        look.y = std::min(MAX_PITCH, look.y);

        camera_.SetLook(look);
    }

    // jump
    {
        if (jump && is_grounded_) {
            is_grounded_ = false;
            last_ground_time_ = now + 0.3;
        }
    }

    // move
    {
        glm::vec2 m(forward_right * (flight_mode_ ? MOVE_SPEED_FLIGHT_MODE : MOVE_SPEED));

        const glm::vec2& look = camera_.GetLook();
        const float yaw = look.x;
        const float pitch = look.y;
        const float sin_yaw = std::sin(yaw);
        const float cos_yaw = std::cos(yaw);
        const float sin_pitch = std::sin(pitch);
        const float cos_pitch = std::cos(pitch);

        glm::vec3 move(
            cos_pitch * sin_yaw * m[0] + -cos_yaw * m[1],
            flight_mode_ ? sin_pitch * m[0] : (now - last_ground_time_) * GRAVITY,
            cos_pitch * cos_yaw * m[0] + sin_yaw * m[1]);

        if (collision_) {
            auto response = movement_controller_.CalculateMovement(engine_chunk_data_, camera_.GetEye(),
                move + glm::vec3(0, 0, 0) // fix -0.0 values
            );

            if (response.axis & math::CollisionAxis::kY) {
                if (!is_grounded_) {
                    LOG(DEBUG) << "grounded";
                }

                is_grounded_ = true;
                last_ground_time_ = now;
            } else {
                if (is_grounded_) {
                    // if was grounded but now no Y collsion
                    LOG(DEBUG) << "fly";
                }

                is_grounded_ = false;
            }

            camera_.SetEye(response.new_camera_pos);
        } else {
            camera_.SetEye(camera_.GetEye() + move);
        }
    }

    engine_render_data_.pointing_block = movement_controller_.PointingBlock(engine_chunk_data_, math::fLine3(camera_.GetEye(), camera_.GetViewDirection() * 4.f));
}

} // namespace chunklands::engine::character