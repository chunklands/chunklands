
#include "Chunk.hxx"
#include <chunklands/modules/misc/misc_module.hxx>

namespace chunklands::game {

  Chunk::Chunk(glm::ivec3 pos) : pos_(std::move(pos)) {
  }

  Chunk::~Chunk() {
    Cleanup();
  }

  void Chunk::InitializeGL() {
    CC_ASSERT(state_ != kViewPrepared);

    Cleanup();

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
    CC_ASSERT(state_ == kViewPrepared);
    CHECK_GL();

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vb_index_count_);
  }

} // namespace chunklands::game