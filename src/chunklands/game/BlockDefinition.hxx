#ifndef __CHUNKLANDS_GAME_BLOCKDEFINITION_HXX__
#define __CHUNKLANDS_GAME_BLOCKDEFINITION_HXX__

#include <string>
#include <unordered_map>
#include <vector>
#include <chunklands/gl/glfw.hxx>
#include <chunklands/engine/ICollisionSystem.hxx>

namespace chunklands::game {

  class BlockDefinition {
  public:
    BlockDefinition(std::string id, bool opaque, std::unordered_map<std::string, std::vector<GLfloat>> faces_vertex_data);

  public:
    const std::string& GetId() const {
      return id_;
    }

    bool IsOpaque() const {
      return opaque_;
    }

    const std::unordered_map<std::string, std::vector<GLfloat>>& GetFacesVertexData() const {
      return faces_vertex_data_;
    }

    engine::collision_result ProcessCollision(const math::ivec3& block_coord, const math::fAABB3& box, const math::fvec3& movement) const;

  private:
    std::string id_;
    bool opaque_ = false;
    std::unordered_map<std::string, std::vector<GLfloat>> faces_vertex_data_;
  };

} // namespace chunklands::game

#endif