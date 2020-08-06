
#include "Camera.hxx"

#include <cmath>

namespace chunklands::engine {

  glm::vec3 Camera::GetCenter() const {
    const float sin_yaw = std::sin(yaw_);
    const float cos_yaw = std::cos(yaw_);
    const float sin_pitch = std::sin(pitch_);
    const float cos_pitch = std::cos(pitch_);

    glm::vec3 center = eye_; // copy
    center.x += -sin_yaw * cos_pitch;
    center.y += sin_pitch;
    center.z += cos_yaw * cos_pitch;

    return center;
  }

} // namespace chunklands::engine