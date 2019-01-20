#include "BlockDefinition.h"

namespace chunklands {
  BlockDefinition::BlockDefinition(std::string id, bool opaque, std::vector<GLfloat> vertex_data)
    : id_(std::move(id)), opaque_(opaque), vertex_data_(std::move(vertex_data)) {

  }
}
