#ifndef __CHUNKLANDS_ENGINE_CHARACTER_CHARACTERCONTROLLER_HXX__
#define __CHUNKLANDS_ENGINE_CHARACTER_CHARACTERCONTROLLER_HXX__

#include <chunklands/engine/camera/Camera.hxx>
#include <chunklands/engine/collision/MovementController.hxx>
#include <glm/vec2.hpp>

namespace chunklands::engine::character {

class CharacterController {
public:
    CharacterController(camera::Camera& camera, EngineChunkData& engine_chunk_data)
        : camera_(camera)
        , engine_chunk_data_(engine_chunk_data)
    {
    }

    // void Jump();
    void Move(float forward, float right);
    void Look(const glm::vec2& delta);

private:
    camera::Camera& camera_;
    EngineChunkData& engine_chunk_data_;
    // bool is_grounded = false;

    collision::MovementController movement_controller_;
};

} // namespace chunklands::engine::character

#endif