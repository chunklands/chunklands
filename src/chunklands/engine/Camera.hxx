#ifndef __CHUNKLANDS_ENGINE_CAMERA_HXX__
#define __CHUNKLANDS_ENGINE_CAMERA_HXX__

#include <glm/vec3.hpp>

namespace chunklands::engine {

  class Camera {
  public:
    const glm::vec3& GetEye() const {
      return eye_;
    }

    glm::vec3 GetCenter() const;

  private:
    glm::vec3 eye_{10, 40, -50};
    float yaw_ = 0;
    float pitch_ = -0.2f;
  };

} // namespace chunklands::engine

#endif