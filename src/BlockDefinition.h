#ifndef __CHUNKLANDS_BLOCKDEFINITION_H__
#define __CHUNKLANDS_BLOCKDEFINITION_H__

#include <string>
#include <vector>
#include <unordered_map>
#include "gl.h"

namespace chunklands {
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

  private:
    std::string id_;
    bool opaque_ = false;
    std::unordered_map<std::string, std::vector<GLfloat>> faces_vertex_data_;
  };
}

#endif
