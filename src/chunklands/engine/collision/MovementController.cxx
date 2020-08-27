
#include "MovementController.hxx"

#include "collision.hxx"
#include <chunklands/engine/chunk/Chunk.hxx>
#include <chunklands/engine/math/math.hxx>

namespace chunklands::engine::collision {

collision::collision_impulse ProcessCollision(block::Block* block, const math::ivec3& block_coord, const math::fAABB3& box, const math::fvec3& movement)
{
    assert(block);

    // collision with opaque blocks
    if (!block->opaque) {
        return collision::collision_impulse();
    }

    return collision::collision_impulse(
        collision::AABB3D { glm::vec3(block_coord), glm::vec3(1) },
        collision::AABB3D { box.origin, box.span },
        movement);
}

collision_impulse ProcessNextCollision(const EngineChunkData& data, const math::fAABB3& box, const math::fvec3& movement)
{
    auto& by_pos = data.GetChunksByPos();
    math::fAABB3 movement_box { box | movement };

    collision::collision_impulse result;

    for (auto&& chunk_pos : math::chunk_pos_in_box { movement_box, CE_CHUNK_SIZE }) {
        const auto&& chunk_result = by_pos.find(chunk_pos);
        if (chunk_result == by_pos.cend()) {
            // TODO(daaitch): make collision with chunk
            std::cout << "not loaded: " << chunk_pos << std::endl;
            continue;
        }

        auto&& chunk = chunk_result->second;
        if (chunk->state < chunk::ChunkState::kDataPrepared) {
            continue;
        }
        math::ivec3 chunk_coord { chunk_pos * (int)CE_CHUNK_SIZE };

        for (auto&& block_pos : math::block_pos_in_box { movement_box, chunk_pos, CE_CHUNK_SIZE }) {
            block::Block* block = chunk->blocks[block_pos.z][block_pos.y][block_pos.x];
            assert(block != nullptr);

            math::ivec3 block_coord { chunk_coord + block_pos };

            const collision::collision_impulse impulse = ProcessCollision(block, block_coord, box, movement);
            if (impulse.is_more_relevant_than(result)) {
                result = impulse;
            }
        }
    }

    return result;
}

movement_response
MovementController::CalculateMovement(const EngineChunkData& data, glm::vec3 camera_pos, glm::vec3 outstanding_movement)
{
    int axis = math::CollisionAxis::kNone;
    int next_collision_index = 0;
    while (math::chess_distance(outstanding_movement, math::fvec3(0, 0, 0)) > 0.f) {
        // infinite loop?
        assert(next_collision_index < 100);

        auto&& impulse = ProcessNextCollision(data, player_box_ + camera_pos, outstanding_movement);
        if (impulse.collision.never() || !impulse.collision.in_unittime()) {
            camera_pos += outstanding_movement;
            break;
        }

        outstanding_movement = impulse.outstanding;
        camera_pos += impulse.collision_free;

        axis |= impulse.is_x_collision ? math::CollisionAxis::kX : math::CollisionAxis::kNone;
        axis |= impulse.is_y_collision ? math::CollisionAxis::kY : math::CollisionAxis::kNone;
        axis |= impulse.is_z_collision ? math::CollisionAxis::kZ : math::CollisionAxis::kNone;

        ++next_collision_index;
    }

    return {
        .axis = axis,
        .new_camera_pos = camera_pos
    };
}
} // namespace chunklands::engine::collision