#ifndef __CHUNKLANDS_BLOCKDEFINITION_H__
#define __CHUNKLANDS_BLOCKDEFINITION_H__

#include <string>
#include <vector>
#include "gl.h"

namespace chunklands {
  class BlockDefinition {
  public:
    BlockDefinition(std::string id, bool opaque, std::vector<GLfloat> vertex_data);

  public:
    const std::string& GetId() const {
      return id_;
    }

    bool IsOpaque() const {
      return opaque_;
    }

    const std::vector<GLfloat>& GetVertexData() const {
      return vertex_data_;
    }

  private:
    std::string id_;
    bool opaque_ = false;
    std::vector<GLfloat> vertex_data_;
  };
}

#endif
