#ifndef __CHUNKLANDS_ENGINE_CHARACTERCONTROLLER_HXX__
#define __CHUNKLANDS_ENGINE_CHARACTERCONTROLLER_HXX__

#include "Camera.hxx"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace chunklands::engine {

class CharacterController {
public:
    CharacterController(Camera* camera)
        : camera_(camera)
    {
        assert(camera_);
    }

    // void Jump();
    void Move(float forward, float right);
    void Look(const glm::vec2& delta);

private:
    Camera* camera_ = nullptr;
    // bool is_grounded = false;
};

} // namespace chunklands::engine

#endif