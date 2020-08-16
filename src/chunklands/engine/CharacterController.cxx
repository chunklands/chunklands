
#include "CharacterController.hxx"
#include <cmath>

namespace chunklands::engine {

  constexpr float LOOK_RADIANS = M_PI * 0.9;
  constexpr float MIN_PITCH = -LOOK_RADIANS / 2.0;
  constexpr float MAX_PITCH = +LOOK_RADIANS / 2.0;

  void CharacterController::Move(const float forward, const float right) {
    // TODO(daaitch): collision here
    const glm::vec2& look = camera_->GetLook();
    const float yaw = look.x;
    const float pitch = look.y;
    const float sin_yaw = std::sin(yaw);
    const float cos_yaw = std::cos(yaw);
    const float sin_pitch = std::sin(pitch);
    const float cos_pitch = std::cos(pitch);

    glm::vec3 move(
      cos_pitch * sin_yaw * forward + -cos_yaw * right,
      sin_pitch * forward,
      cos_pitch * cos_yaw * forward + sin_yaw * right
    );

    camera_->SetEye(camera_->GetEye() + move);
  }
  
  void CharacterController::Look(const glm::vec2& delta) {
    glm::vec2 look = camera_->GetLook() + delta;

    // yaw modulo
    look.x = std::fmod(look.x, float(2.f * M_PI));

    // pitch range
    look.y = std::max(MIN_PITCH, look.y);
    look.y = std::min(MAX_PITCH, look.y);

    camera_->SetLook(look);
  }

} // namespace chunklands::engine