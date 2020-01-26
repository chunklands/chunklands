#include "BlockDefinition.h"

namespace chunklands {
  BlockDefinition::BlockDefinition(std::string id, bool opaque, std::unordered_map<std::string, std::vector<GLfloat>> faces_vertex_data)
    : id_(std::move(id)), opaque_(opaque), faces_vertex_data_(std::move(faces_vertex_data)) {

  }
}
