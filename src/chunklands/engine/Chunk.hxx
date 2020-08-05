#ifndef __CHUNKLANDS_ENGINE_SCENE_CHUNK_HXX__
#define __CHUNKLANDS_ENGINE_SCENE_CHUNK_HXX__

#include <cstring>
#include "api-types.hxx"
#include "GBufferMesh.hxx"
#include "Block.hxx"

namespace chunklands::engine {

  enum ChunkState {
    kEmpty,
    kDataPrepared,
    kMeshPrepared,
    kCount,
  };

  static_assert(CE_CHUNK_STATE_COUNT == ChunkState::kCount, "check");

  struct Chunk {
    Chunk(int x, int y, int z) : x(x), y(y), z(z) {
      std::memset(&blocks, 0, sizeof(blocks));
    }

    int x, y, z;
    ChunkState state = ChunkState::kEmpty;
    Block* blocks[CE_CHUNK_SIZE][CE_CHUNK_SIZE][CE_CHUNK_SIZE];

    GBufferMesh mesh;
  };

} // namespace chunklands::engine

#endif