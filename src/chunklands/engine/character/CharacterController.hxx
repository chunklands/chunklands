#ifndef __CHUNKLANDS_ENGINE_CHARACTER_CHARACTERCONTROLLER_HXX__
#define __CHUNKLANDS_ENGINE_CHARACTER_CHARACTERCONTROLLER_HXX__

#include <chunklands/engine/EngineRenderData.hxx>
#include <chunklands/engine/camera/Camera.hxx>
#include <chunklands/engine/collision/MovementController.hxx>
#include <glm/vec2.hpp>

namespace chunklands::engine::character {

class CharacterController {
public:
    CharacterController(
        camera::Camera& camera, EngineChunkData& engine_chunk_data, EngineRenderData& engine_render_data)
        : camera_(camera)
        , engine_chunk_data_(engine_chunk_data)
        , engine_render_data_(engine_render_data)
    {
    }

    // void Jump();
    void MoveAndLook(const glm::vec2& forward_right, const glm::vec2& look_delta);

    void SetCollision(bool collision)
    {
        collision_ = collision;
    }

    bool IsCollision() const
    {
        return collision_;
    }

private:
    camera::Camera& camera_;
    EngineChunkData& engine_chunk_data_;
    EngineRenderData& engine_render_data_;
    // bool is_grounded = false;

    collision::MovementController movement_controller_;
    bool collision_ = true;
};

} // namespace chunklands::engine::character

#endif