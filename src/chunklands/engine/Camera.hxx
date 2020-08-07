#ifndef __CHUNKLANDS_ENGINE_CAMERA_HXX__
#define __CHUNKLANDS_ENGINE_CAMERA_HXX__

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace chunklands::engine {

  class Camera {
  public:
    const glm::vec3& GetEye() const {
      return eye_;
    }

    void SetEye(const glm::vec3& eye) {
      eye_ = eye;
    }

    const glm::vec2& GetLook() const {
      return look_;
    }

    void SetLook(const glm::vec2& look) {
      look_ = look;
    }

    glm::vec3 GetViewDirection() const;
    glm::vec3 GetCenter() const;

  private:
    glm::vec3 eye_{10, 40, -50};
    glm::vec2 look_{0, -0.2f};
  };

} // namespace chunklands::engine

#endif