#include "Chunk.h"
#include <vector>

namespace chunklands {

  

  Chunk::Chunk(glm::ivec3 pos) : pos_(std::move(pos)) {
  }

  Chunk::~Chunk() {
    Cleanup();
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

    // blocks don't have to be freed and will not reset them
    state_ = kEmpty;
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

  void Chunk::PrepareView(const Chunk* neighbors[kNeighborCount]) {
    assert(state_ == kModelPrepared);
    assert(neighbors[kLeft  ] != nullptr && neighbors[kLeft  ]->GetState() >= kModelPrepared);
    assert(neighbors[kRight ] != nullptr && neighbors[kRight ]->GetState() >= kModelPrepared);
    assert(neighbors[kTop   ] != nullptr && neighbors[kTop   ]->GetState() >= kModelPrepared);
    assert(neighbors[kBottom] != nullptr && neighbors[kBottom]->GetState() >= kModelPrepared);
    assert(neighbors[kFront ] != nullptr && neighbors[kFront ]->GetState() >= kModelPrepared);
    assert(neighbors[kBack  ] != nullptr && neighbors[kBack  ]->GetState() >= kModelPrepared);

    // allocate client side vertex buffer
    std::vector<GLfloat> vertex_buffer_data;
    vertex_buffer_data.reserve(estimated_floats_in_buffer);

    glm::vec3 chunk_offset(glm::vec3(pos_) * (float)SIZE);

    ForEachBlock([&](BlockType block_type, int x, int y, int z) {
      // skip AIR
      if (block_type == 0) {
        return;
      }

      // for all sides of the block, test if adjacent block is opaque
      if (
      //                      << inside of chunk || check neighbor >>
        ((x >= 1     && blocks_[z][y][x-1] != 0) || (x == 0      && neighbors[kLeft  ]->blocks_[z][y][SIZE-1] != 0)) && // left
        ((x < SIZE-1 && blocks_[z][y][x+1] != 0) || (x == SIZE-1 && neighbors[kRight ]->blocks_[z][y][0]      != 0)) && // right
        ((y >= 1     && blocks_[z][y-1][x] != 0) || (y == 0      && neighbors[kBottom]->blocks_[z][SIZE-1][x] != 0)) && // bottom
        ((y < SIZE-1 && blocks_[z][y+1][x] != 0) || (y == SIZE-1 && neighbors[kTop   ]->blocks_[z][0][x]      != 0)) && // top
        ((z >= 1     && blocks_[z-1][y][x] != 0) || (z == 0      && neighbors[kFront ]->blocks_[SIZE-1][y][x] != 0)) && // front
        ((z < SIZE-1 && blocks_[z+1][y][x] != 0) || (z == SIZE-1 && neighbors[kBack  ]->blocks_[0][y][x]      != 0)))   // back
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

    vb_vertex_count_ = vertex_buffer_data.size();

    // copy vector to graphic card
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vb_);

    glBindBuffer(GL_ARRAY_BUFFER, vb_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_buffer_data.size(),
                 vertex_buffer_data.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao_);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    CHECK_GL();

    state_ = kViewPrepared;
  }

  void Chunk::Render() {
    assert(state_ == kViewPrepared);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vb_vertex_count_);
    CHECK_GL();
  }
}