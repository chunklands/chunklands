
#include "Chunk.hxx"
#include <chunklands/misc/flow.hxx>

namespace chunklands::game {

  Chunk::Chunk(glm::ivec3 pos) : pos_(std::move(pos)) {
    for (unsigned z = 0; z < Chunk::SIZE; z++) {
      for (unsigned y = 0; y < Chunk::SIZE; y++) {
        for (unsigned x = 0; x < Chunk::SIZE; x++) {
          blocks_[z][y][x] = nullptr;
        }
      }
    }
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

  void Chunk::UpdateBlock(const glm::ivec3& at, BlockDefinition* block_def) {
    assert(block_def);

    assert(at.x >= 0 && at.y >= 0 && at.z >= 0);
    assert(at.x < (int)SIZE && at.y < (int)SIZE && at.z < (int)SIZE);

    blocks_[at.z][at.y][at.x] = block_def;
    state_ = ChunkState::kModelPrepared;
  }

} // namespace chunklands::game