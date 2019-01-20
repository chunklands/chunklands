#include "ChunkGeneratorBase.h"

#include <cmath>

namespace chunklands {
  DEFINE_OBJECT_WRAP_DEFAULT_CTOR(ChunkGeneratorBase, ONE_ARG({
    
  }))

  namespace {
    constexpr float Ax      = 12.f;
    constexpr float omega_x = (2.f * M_PI) / 31.f;
    constexpr float phi_x   = (2.f * M_PI) / 10.f;
    constexpr float Az      = 9.f;
    constexpr float omega_z = (2.f * M_PI) / 44.f;
    constexpr float phi_z   = (2.f * M_PI) / 27.f;

    bool IsGroundMountains(const glm::ivec3& pos) {
      return pos.y < (
          (Ax * sinf(omega_x * pos.x + phi_x))
          + (Az * sinf(omega_z * pos.z + phi_z))
      );
    }

    bool IsGroundFlat(const glm::ivec3& pos) {
      return pos.y < 4;
    }

    bool IsGroundAbstract1(const glm::ivec3& pos) {
      return pos.y * pos.y + pos.x < pos.y;
    }

    bool IsGroundSphere(const glm::ivec3& pos) {
      return pos.x * pos.x + pos.y * pos.y + pos.z * pos.z < 30*30;
    }

  }

  void ChunkGeneratorBase::GenerateModel(Chunk& chunk) {
    assert(chunk.GetState() == kEmpty);
    auto&& pos = chunk.GetPos();

    chunk.ForEachBlock([&](char& block_type, int x, int y, int z) {
      glm::ivec3 abs_pos((int)Chunk::SIZE * pos + glm::ivec3(x, y, z));
      block_type = IsGroundMountains(abs_pos) ? 1 : 0;
    });

    chunk.state_ = kModelPrepared;
  }


  constexpr int POSITION_VERTICES_IN_BLOCK = 6 * 2 * 3;
  constexpr int NORMAL_VERTICES_IN_BLOCK = POSITION_VERTICES_IN_BLOCK;
  constexpr int FLOATS_IN_BLOCK = (POSITION_VERTICES_IN_BLOCK + NORMAL_VERTICES_IN_BLOCK) * 3;
  GLfloat BLOCK_DATA[FLOATS_IN_BLOCK] = {
    // VERTEX       NORMAL            VERTEX          NORMAL            VERTEX          NORMAL

    // front
    0.f, 1.f, 0.f,  0.f, 0.f, -1.f,   0.f, 0.f, 0.f,  0.f, 0.f, -1.f,   1.f, 0.f, 0.f,  0.f, 0.f, -1.f,
    0.f, 1.f, 0.f,  0.f, 0.f, -1.f,   1.f, 0.f, 0.f,  0.f, 0.f, -1.f,   1.f, 1.f, 0.f,  0.f, 0.f, -1.f,

    // back
    1.f, 1.f, 1.f,  0.f, 0.f, +1.f,   1.f, 0.f, 1.f,  0.f, 0.f, +1.f,   0.f, 0.f, 1.f,  0.f, 0.f, +1.f,
    1.f, 1.f, 1.f,  0.f, 0.f, +1.f,   0.f, 0.f, 1.f,  0.f, 0.f, +1.f,   0.f, 1.f, 1.f,  0.f, 0.f, +1.f,

    // left
    0.f, 1.f, 1.f,  -1.f, 0.f, 0.f,   0.f, 0.f, 1.f,  -1.f, 0.f, 0.f,   0.f, 0.f, 0.f,  -1.f, 0.f, 0.f,
    0.f, 1.f, 1.f,  -1.f, 0.f, 0.f,   0.f, 0.f, 0.f,  -1.f, 0.f, 0.f,   0.f, 1.f, 0.f,  -1.f, 0.f, 0.f,

    // right
    1.f, 1.f, 0.f,  +1.f, 0.f, 0.f,   1.f, 0.f, 0.f,  +1.f, 0.f, 0.f,   1.f, 0.f, 1.f,  +1.f, 0.f, 0.f,
    1.f, 1.f, 0.f,  +1.f, 0.f, 0.f,   1.f, 0.f, 1.f,  +1.f, 0.f, 0.f,   1.f, 1.f, 1.f,  +1.f, 0.f, 0.f,

    // top
    0.f, 1.f, 1.f,  0.f, +1.f, 0.f,   0.f, 1.f, 0.f,  0.f, +1.f, 0.f,   1.f, 1.f, 0.f,  0.f, +1.f, 0.f,
    0.f, 1.f, 1.f,  0.f, +1.f, 0.f,   1.f, 1.f, 0.f,  0.f, +1.f, 0.f,   1.f, 1.f, 1.f,  0.f, +1.f, 0.f,

    // bottom
    0.f, 0.f, 0.f,  0.f, -1.f, 0.f,   0.f, 0.f, 1.f,  0.f, -1.f, 0.f,   1.f, 0.f, 1.f,  0.f, -1.f, 0.f,
    0.f, 0.f, 0.f,  0.f, -1.f, 0.f,   1.f, 0.f, 1.f,  0.f, -1.f, 0.f,   1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
  };

  // for now just an estimation: allocation vs. growing vector
  constexpr int estimated_block_count = Chunk::SIZE * Chunk::SIZE * Chunk::SIZE / 2;
  constexpr int estimated_floats_in_buffer = estimated_block_count * FLOATS_IN_BLOCK;

  void ChunkGeneratorBase::GenerateView(Chunk& chunk, const Chunk* neighbors[kNeighborCount]) {
    assert(chunk.GetState() == kModelPrepared);
    assert(neighbors[kLeft  ] != nullptr && neighbors[kLeft  ]->GetState() >= kModelPrepared);
    assert(neighbors[kRight ] != nullptr && neighbors[kRight ]->GetState() >= kModelPrepared);
    assert(neighbors[kTop   ] != nullptr && neighbors[kTop   ]->GetState() >= kModelPrepared);
    assert(neighbors[kBottom] != nullptr && neighbors[kBottom]->GetState() >= kModelPrepared);
    assert(neighbors[kFront ] != nullptr && neighbors[kFront ]->GetState() >= kModelPrepared);
    assert(neighbors[kBack  ] != nullptr && neighbors[kBack  ]->GetState() >= kModelPrepared);

    // allocate client side vertex buffer
    std::vector<GLfloat> vertex_buffer_data;
    vertex_buffer_data.reserve(estimated_floats_in_buffer);

    glm::vec3 chunk_offset(glm::vec3(chunk.GetPos()) * (float)Chunk::SIZE);

    chunk.ForEachBlock([&](Chunk::BlockType block_type, int x, int y, int z) {
      // skip AIR
      if (block_type == 0) {
        return;
      }

      // for all sides of the block, test if adjacent block is opaque
      if (
      //                                       << inside of chunk || check neighbor >>
        ((x >= 1            && chunk.blocks_[z][y][x-1] != 0) || (x == 0             && neighbors[kLeft  ]->blocks_[z][y][Chunk::SIZE-1] != 0)) && // left
        ((x < Chunk::SIZE-1 && chunk.blocks_[z][y][x+1] != 0) || (x == Chunk::SIZE-1 && neighbors[kRight ]->blocks_[z][y][0]             != 0)) && // right
        ((y >= 1            && chunk.blocks_[z][y-1][x] != 0) || (y == 0             && neighbors[kBottom]->blocks_[z][Chunk::SIZE-1][x] != 0)) && // bottom
        ((y < Chunk::SIZE-1 && chunk.blocks_[z][y+1][x] != 0) || (y == Chunk::SIZE-1 && neighbors[kTop   ]->blocks_[z][0][x]             != 0)) && // top
        ((z >= 1            && chunk.blocks_[z-1][y][x] != 0) || (z == 0             && neighbors[kFront ]->blocks_[Chunk::SIZE-1][y][x] != 0)) && // front
        ((z < Chunk::SIZE-1 && chunk.blocks_[z+1][y][x] != 0) || (z == Chunk::SIZE-1 && neighbors[kBack  ]->blocks_[0][y][x]             != 0)))   // back
      {
        return;
      }

      static_assert(FLOATS_IN_BLOCK % 6 == 0,
                    "loop needs floats divisible by 6");
      for (int fi = 0; fi < FLOATS_IN_BLOCK; ) { // float index

        // position vertices
        vertex_buffer_data.push_back(BLOCK_DATA[fi++] + (GLfloat)x + chunk_offset.x);
        vertex_buffer_data.push_back(BLOCK_DATA[fi++] + (GLfloat)y + chunk_offset.y);
        vertex_buffer_data.push_back(BLOCK_DATA[fi++] + (GLfloat)z + chunk_offset.z);

        // normal vertices
        vertex_buffer_data.push_back(BLOCK_DATA[fi++]);
        vertex_buffer_data.push_back(BLOCK_DATA[fi++]);
        vertex_buffer_data.push_back(BLOCK_DATA[fi++]);
      }
    });

    
    chunk.vb_index_count_ = vertex_buffer_data.size();

    // copy vector to graphic card
    chunk.InitializeGL();

    glBindBuffer(GL_ARRAY_BUFFER, chunk.vb_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_buffer_data.size(),
                 vertex_buffer_data.data(), GL_STATIC_DRAW);

    glBindVertexArray(chunk.vao_);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    CHECK_GL();

    chunk.state_ = kViewPrepared;
  }
}
