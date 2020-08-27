#ifndef __CHUNKLANDS_ENGINE_CHARACTER_CHARACTERCONTROLLER_HXX__
#define __CHUNKLANDS_ENGINE_CHARACTER_CHARACTERCONTROLLER_HXX__

#include <chunklands/engine/camera/Camera.hxx>
#include <glm/vec2.hpp>

namespace chunklands::engine::character {

class CharacterController {
public:
    CharacterController(camera::Camera* camera)
        : camera_(camera)
    {
        assert(camera_);
    }

    // void Jump();
    void Move(float forward, float right);
    void Look(const glm::vec2& delta);

private:
    camera::Camera* camera_ = nullptr;
    // bool is_grounded = false;
};

} // namespace chunklands::engine::character

#endif