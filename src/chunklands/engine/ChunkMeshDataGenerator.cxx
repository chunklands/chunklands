
#include "ChunkMeshDataGenerator.hxx"

namespace chunklands::engine {

  inline bool is_opaque(const Chunk* chunk, int bx, int by, int bz) {
    return chunk->blocks[bz][by][bx]->opaque;
  }

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
            if (face.type == FaceType::Left) {
              if (x >   0               && is_opaque(chunk,                           x-1,              y,                z               )) continue;
              if (x ==  0               && is_opaque(neighbors[kChunkNeighborLeft],   CE_CHUNK_SIZE-1,  y,                z               )) continue;
            }

            if (face.type == FaceType::Right) {
              if (x <   CE_CHUNK_SIZE-1 && is_opaque(chunk,                           x+1,              y,                z               )) continue;
              if (x ==  CE_CHUNK_SIZE-1 && is_opaque(neighbors[kChunkNeighborRight],  0,                y,                z               )) continue;
            }
            
            if (face.type == FaceType::Bottom) {
              if (y >   0               && is_opaque(chunk,                           x,                y-1,              z               )) continue;
              if (y ==  0               && is_opaque(neighbors[kChunkNeighborBottom], x,                CE_CHUNK_SIZE-1,  z               )) continue;
            }

            if (face.type == FaceType::Top) {
              if (y <   CE_CHUNK_SIZE-1 && is_opaque(chunk,                           x,                y+1,              z               )) continue;
              if (y ==  CE_CHUNK_SIZE-1 && is_opaque(neighbors[kChunkNeighborTop],    x,                0,                z               )) continue;
            }

            if (face.type == FaceType::Front) {
              if (z >   0               && is_opaque(chunk,                           x,                y,                z-1             )) continue;
              if (z ==  0               && is_opaque(neighbors[kChunkNeighborFront],  x,                y,                CE_CHUNK_SIZE-1 )) continue;
            }

            if (face.type == FaceType::Back) {
              if (z <   CE_CHUNK_SIZE-1 && is_opaque(chunk,                           x,                y,                z+1             )) continue;
              if (z ==  CE_CHUNK_SIZE-1 && is_opaque(neighbors[kChunkNeighborBack],   x,                y,                0               )) continue;
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