#ifndef __CHUNKLANDS_ENGINE_CAMERA_HXX__
#define __CHUNKLANDS_ENGINE_CAMERA_HXX__

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <boost/signals2.hpp>
#include "engine_types.hxx"

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

    void ProcessEvents() {
      if (eye_ != last_eye_) {
        on_position_change({
          .x = eye_.x,
          .y = eye_.y,
          .z = eye_.z
        });
        
        last_eye_ = eye_;
      }
    }

  public:
    boost::signals2::signal<void(CECameraPosition)> on_position_change;

  private:
    glm::vec3 eye_{16, 16, 16};
    glm::vec2 look_{0, -0.2f};

    glm::vec3 last_eye_ = eye_;
  };

} // namespace chunklands::engine

#endif