
#include "BlockDefinition.hxx"
#include <chunklands/debug.hxx>
#include <iostream>

namespace chunklands::game {

  BlockDefinition::BlockDefinition(std::string id, bool opaque, std::unordered_map<std::string, std::vector<GLfloat>> faces_vertex_data)
    : id_(std::move(id)), opaque_(opaque), faces_vertex_data_(std::move(faces_vertex_data)) {
  }

  collision::collision_impulse BlockDefinition::ProcessCollision(const math::ivec3& block_coord, const math::fAABB3& box, const math::fvec3& movement) const {
    // collision with opaque blocks
    if (!opaque_) {
      return collision::collision_impulse();
    }

    return collision::collision_impulse(
      collision::AABB3D{glm::vec3(block_coord), glm::vec3(1)},
      collision::AABB3D{box.origin, box.span},
      movement
    );
  }

} // namespace chunklands::game