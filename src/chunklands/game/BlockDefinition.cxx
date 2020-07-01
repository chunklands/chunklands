
#include "BlockDefinition.hxx"
#include <chunklands/debug.hxx>
#include <iostream>

namespace chunklands::game {

  BlockDefinition::BlockDefinition(std::string id, bool opaque, std::unordered_map<std::string, std::vector<GLfloat>> faces_vertex_data)
    : id_(std::move(id)), opaque_(opaque), faces_vertex_data_(std::move(faces_vertex_data)) {
  }

  engine::collision_result BlockDefinition::ProcessCollision(const math::ivec3& block_coord, const math::fAABB3& box, const math::fvec3& movement) const {
    if (!opaque_) {
      return {
        .prio = 0,
        .axis = math::CollisionAxis::kNone,
        .ctime = 1,
        .collisionfree_movement{movement},
        .outstanding_movement{0,0,0}
      };
    }

    math::fAABB3 block_box{ block_coord, math::fvec3{1, 1, 1} };
    auto&& collision = math::collision_3d(box, movement, block_box);

    if (!collision.time) {
      return {
        .prio = 0,
        .axis = math::CollisionAxis::kNone,
        .ctime = 1,
        .collisionfree_movement{movement},
        .outstanding_movement{0,0,0}
      };
    }
    
    if (collision.time.origin.x < 0) {
      return {
        .prio = 0,
        .axis = collision.axis,
        .ctime = 0,
        .collisionfree_movement{0, 0, 0},
        .outstanding_movement{0, 0, 0}
      };
    }

    if (collision.time.origin.x > 1) {
      return {
        .prio = 0,
        .axis = collision.axis,
        .ctime = 1,
        .collisionfree_movement{movement},
        .outstanding_movement{0,0,0}
      };
    }

    if (DEBUG_COLLISION) {
      std::cout << collision << std::endl;
    }

    float ctime = collision.time.origin.x;
    math::fvec3 good_movement = ctime * movement;
    math::fvec3 bad_movement = movement - good_movement;

    int prio = 0;

    if (collision.axis & math::CollisionAxis::kX) {
      bad_movement.x = 0;
      prio++;
    }

    if (collision.axis & math::CollisionAxis::kY) {
      bad_movement.y = 0;
      prio++;
    }

    if (collision.axis & math::CollisionAxis::kZ) {
      bad_movement.z = 0;
      prio++;
    }

    return {
      .prio = prio,
      .axis = collision.axis,
      .ctime = ctime,
      .collisionfree_movement{good_movement},
      .outstanding_movement{bad_movement}
    };
  }

} // namespace chunklands::game