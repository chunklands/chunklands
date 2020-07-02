#ifndef __CHUNKLANDS_GAME_CHUNK_HXX__
#define __CHUNKLANDS_GAME_CHUNK_HXX__

#include "BlockDefinition.hxx"
#include <array>
#include <chunklands/gl/glfw.hxx>

namespace chunklands::game {

  enum ChunkState {
    kEmpty,
    kModelIsPreparing,
    kModelPrepared,
    kViewPrepared
  };

  enum ChunkNeighbor {
    kLeft,   // -x
    kRight,  // +x
    kTop,    // +y
    kBottom, // -y
    kFront,  // +z
    kBack,   // -z

    kNeighborCount = 6
  };
  class ChunkGenerator;
  
  class Chunk {
    friend ChunkGenerator;

  public:
    static constexpr unsigned SIZE_LB = 4;
    static constexpr unsigned SIZE = 1 << SIZE_LB;
    static constexpr unsigned BLOCK_COUNT = SIZE * SIZE * SIZE;

  public:
    using BlockType = BlockDefinition*;
    using BlocksType = std::array<std::array<std::array<BlockType, SIZE>, SIZE>, SIZE>;
  
  public:
    Chunk(glm::ivec3 pos);
    ~Chunk();
  
  public:
    void InitializeGL();
    void Cleanup();
    void Render();

    template <typename CbFn>
    void ForEachBlock(const CbFn& fn);

    GLuint GetIndexCount() const {
      return vb_index_count_;
    }

    ChunkState GetState() const {
      return state_;
    }

    const glm::ivec3& GetPos() const {
      return pos_;
    }

    inline const BlockDefinition* BlockAt(const glm::uvec3& at) const {
      assert(at.x < SIZE && at.y < SIZE && at.z < SIZE);
      return blocks_[at.z][at.y][at.x];
    }

    inline const BlockDefinition* BlockAt(const glm::ivec3& at) const {
      assert(at.x >= 0 && at.y >= 0 && at.z >= 0);
      assert(at.x < (int)SIZE && at.y < (int)SIZE && at.z < (int)SIZE);
      
      return blocks_[at.z][at.y][at.x];
    }

    void UpdateBlock(const glm::ivec3& at, BlockDefinition* block_def);

  private:
    ChunkState state_ = kEmpty;
    glm::ivec3 pos_;
    BlocksType blocks_;

    GLuint vb_index_count_ = 0;
    GLuint vao_ = 0;
    GLuint vb_ = 0;
  };

  template <typename CbFn>
  void Chunk::ForEachBlock(const CbFn& fn) {
    for (unsigned z = 0; z < SIZE; z++) {
      auto&& items_yx = blocks_[z];
      for (unsigned y = 0; y < SIZE; y++) {
        auto&& items_x = items_yx[y];
        for (unsigned x = 0; x < SIZE; x++) {
          auto&& item = items_x[x];
          fn(item, x, y, z);
        }
      }
    }
  }

} // namespace chunklands::game

#endif