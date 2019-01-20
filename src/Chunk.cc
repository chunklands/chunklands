#include "Chunk.h"
#include <vector>

namespace chunklands {

  Chunk::Chunk(glm::ivec3 pos) : pos_(std::move(pos)) {
  }

  Chunk::~Chunk() {
    Cleanup();
  }

  void Chunk::InitializeGL() {
    assert(vao_ == 0 && vb_ == 0);
    assert(state_ != kViewPrepared);

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vb_);
  }

  void Chunk::Cleanup() {
    if (vb_) {
      glDeleteBuffers(1, &vb_);
      vb_ = 0;
    }

    if (vao_) {
      glDeleteVertexArrays(1, &vao_);
      vao_ = 0;
    }

    // blocks will not be reset, simply overwrite them if chunks should be reused
    if (state_ == kViewPrepared) {
      state_ = kModelPrepared;
    }
  }

  void Chunk::Render() {
    assert(state_ == kViewPrepared);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vb_index_count_);
    CHECK_GL();
  }
}