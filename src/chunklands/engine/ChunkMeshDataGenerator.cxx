
#include "ChunkMeshDataGenerator.hxx"

namespace chunklands::engine {

  void ChunkMeshDataGenerator::operator()() {
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

          for (const CEBlockFace& face : block->faces) {
            if (face.type == kLeft && x >= 1 && chunk->blocks[z][y][x-1]->opaque) {
              continue;
            }

            if (face.type == kRight && x < CE_CHUNK_SIZE - 1 && chunk->blocks[z][y][x+1]->opaque) {
              continue;
            }
            
            if (face.type == kBottom && y >= 1 && chunk->blocks[z][y-1][x]->opaque) {
              continue;
            }

            if (face.type == kTop && y < CE_CHUNK_SIZE - 1 && chunk->blocks[z][y+1][x]->opaque) {
              continue;
            }

            if (face.type == kFront && z >= 1 && chunk->blocks[z-1][y][x]->opaque) {
              continue;
            }

            if (face.type == kBack && z < CE_CHUNK_SIZE - 1 && chunk->blocks[z+1][y][x]->opaque) {
              continue;
            }

            for (CEVaoElementChunkBlock e : face.data) { // copy
              e.position[0] += chunk_offset_x + GLfloat(x);
              e.position[1] += chunk_offset_y + GLfloat(y);
              e.position[2] += chunk_offset_z + GLfloat(z);

              vb_data.push_back(std::move(e));
            }
          }
        }
      }
    }

    chunk->mesh.vao.Initialize(vb_data.data(), vb_data.size());

    chunk->state = ChunkState::kMeshPrepared;
  }

} // namespace chunklands::engine