
#include "Camera.hxx"

#include <cmath>

namespace chunklands::engine {

glm::vec3 Camera::GetViewDirection() const
{
    const float sin_yaw = std::sin(-look_.x);
    const float cos_yaw = std::cos(-look_.x);
    const float sin_pitch = std::sin(look_.y);
    const float cos_pitch = std::cos(look_.y);

    return glm::vec3(-sin_yaw * cos_pitch, sin_pitch, cos_yaw * cos_pitch);
}

glm::vec3 Camera::GetCenter() const
{
    return eye_ + GetViewDirection();
}

} // namespace chunklands::engine