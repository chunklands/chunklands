#ifndef __CHUNKLANDS_CHUNK_H__
#define __CHUNKLANDS_CHUNK_H__

#include <array>
#include <glm/vec3.hpp>
#include "gl.h"

namespace chunklands {
  class Chunk {
  public:
    static constexpr unsigned SIZE_LB = 4;
    static constexpr unsigned SIZE = 1 << SIZE_LB;
    static constexpr unsigned BLOCK_COUNT = SIZE * SIZE * SIZE;

  private:
    using BlockType = char;
    using BlocksType = std::array<std::array<std::array<BlockType, SIZE>, SIZE>, SIZE>;
  
  public:
    Chunk(glm::ivec3 pos);
    ~Chunk();
  
  public:
    void Cleanup();
    void Prepare();
    void Render();

    template <typename CbFn>
    void ForEachBlock(const CbFn& fn);

  private:
    glm::ivec3 pos_;
    BlocksType blocks_;
    GLuint vb_vertex_count_ = 0;

    GLuint vao_ = 0;
    GLuint vb_ = 0;
  };

  template <typename CbFn>
  void Chunk::ForEachBlock(const CbFn& fn) {
    for (int z = 0; z < SIZE; z++) {
      auto&& items_yx = blocks_[z];
      for (int y = 0; y < SIZE; y++) {
        auto&& items_x = items_yx[y];
        for (int x = 0; x < SIZE; x++) {
          auto&& item = items_x[x];
          fn(item, x, y, z);
        }
      }
    }
  }
}

#endif