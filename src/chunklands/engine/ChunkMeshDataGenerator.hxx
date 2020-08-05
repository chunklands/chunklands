#ifndef __CHUNKLANDS_ENGINE_CHUNKMESHDATAGENERATOR_HXX__
#define __CHUNKLANDS_ENGINE_CHUNKMESHDATAGENERATOR_HXX__

#include <cassert>
#include <boost/move/core.hpp>
#include "Chunk.hxx"
#include "gl/Vao.hxx"

namespace chunklands::engine {

  struct ChunkMeshDataGenerator {
    ChunkMeshDataGenerator(Chunk* chunk) : chunk(chunk) {
      assert(chunk != nullptr);
    }

    void operator()() {
      assert(chunk->state == ChunkState::kDataPrepared);

      const GLfloat chunk_offset_x = GLfloat(chunk->x * CE_CHUNK_SIZE);
      const GLfloat chunk_offset_y = GLfloat(chunk->y * CE_CHUNK_SIZE);
      const GLfloat chunk_offset_z = GLfloat(chunk->z * CE_CHUNK_SIZE);

      std::vector<CEVaoElementChunkBlock> vb_data;
      for(int z = 0; z < CE_CHUNK_SIZE; z++) {
        for (int y = 0; y < CE_CHUNK_SIZE; y++) {
          for (int x = 0; x < CE_CHUNK_SIZE; x++) {
            const Block* block = chunk->blocks[z][y][x];
            assert(block != nullptr);

            for (CEVaoElementChunkBlock e : block->data) { // copy
              e.position[0] += chunk_offset_x + GLfloat(x);
              e.position[1] += chunk_offset_y + GLfloat(y);
              e.position[2] += chunk_offset_z + GLfloat(z);

              vb_data.push_back(std::move(e));
            }
            // vb_data.insert(vb_data.end(), block->data.begin(), block->data.end());
          }
        }
      }

      chunk->mesh.vao.Initialize(vb_data.data(), vb_data.size());

      chunk->state = ChunkState::kMeshPrepared;
    }

    Chunk* const chunk;

    BOOST_MOVABLE_BUT_NOT_COPYABLE(ChunkMeshDataGenerator)
  };

} // namespace chunklands::engine

#endif