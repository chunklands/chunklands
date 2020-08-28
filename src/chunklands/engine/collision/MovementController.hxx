#ifndef __CHUNKLANDS_ENGINE_COLLISION_MOVEMENTCONTROLLER_HXX__
#define __CHUNKLANDS_ENGINE_COLLISION_MOVEMENTCONTROLLER_HXX__

#include <chunklands/engine/EngineChunkData.hxx>
#include <chunklands/engine/math/math.hxx>
#include <glm/vec3.hpp>

namespace chunklands::engine::collision {

struct movement_response {
    int axis;
    glm::vec3 new_camera_pos;
};

class MovementController {
public:
    movement_response CalculateMovement(
        const EngineChunkData& data,
        glm::vec3 camera_pos,
        glm::vec3 outstanding_movement);

    std::optional<math::ivec3> PointingBlock(const EngineChunkData& data, const math::fLine3& look);

private:
    math::fAABB3 player_box_ {
        { -.3f, -1.4f, -.3f },
        { .6f, 1.6f, .6f }
    };
};

} // namespace chunklands::engine::collision

#endif