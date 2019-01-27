#include "ChunkGeneratorBase.h"

#include <cmath>

namespace chunklands {
  DEFINE_OBJECT_WRAP_DEFAULT_CTOR(ChunkGeneratorBase, ONE_ARG({
    InstanceMethod("setBlockRegistrar", &ChunkGeneratorBase::SetBlockRegistrar)
  }))

  void ChunkGeneratorBase::SetBlockRegistrar(const Napi::CallbackInfo& info) {
    block_registrar_ = info[0].ToObject();
  }

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

    auto&& air_ptr = block_registrar_->Find("block.air");
    auto&& block_ptr = block_registrar_->Find("block.block");

    assert(air_ptr != nullptr);
    assert(block_ptr != nullptr);

    chunk.ForEachBlock([&](Chunk::BlockType& block_type, int x, int y, int z) {
      glm::ivec3 abs_pos((int)Chunk::SIZE * pos + glm::ivec3(x, y, z));
      block_type = IsGroundMountains(abs_pos) ? block_ptr : air_ptr;
    });

    chunk.state_ = kModelPrepared;
  }

  // for now just an estimation: allocation vs. growing vector
  // faces * triangle_per_face * vertices_per_triangle * floats_per_vertex * 0.5 load
  constexpr int estimated_avg_floats_per_block = (6 * 2 * 3 * 8) / 2;
  constexpr int estimated_floats_in_buffer = Chunk::SIZE * Chunk::SIZE * Chunk::SIZE * estimated_avg_floats_per_block;

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

    chunk.ForEachBlock([&](const Chunk::BlockType& block_type, int x, int y, int z) {
      assert(block_type != nullptr);

      // for all sides of the block, test if adjacent block is opaque
      if (
      //                                          << inside of chunk || check neighbor >>
        ((x >= 1            && chunk.blocks_[z][y][x-1]->IsOpaque()) || (x == 0             && neighbors[kLeft  ]->blocks_[z][y][Chunk::SIZE-1]->IsOpaque())) && // left
        ((x < Chunk::SIZE-1 && chunk.blocks_[z][y][x+1]->IsOpaque()) || (x == Chunk::SIZE-1 && neighbors[kRight ]->blocks_[z][y][0]            ->IsOpaque())) && // right
        ((y >= 1            && chunk.blocks_[z][y-1][x]->IsOpaque()) || (y == 0             && neighbors[kBottom]->blocks_[z][Chunk::SIZE-1][x]->IsOpaque())) && // bottom
        ((y < Chunk::SIZE-1 && chunk.blocks_[z][y+1][x]->IsOpaque()) || (y == Chunk::SIZE-1 && neighbors[kTop   ]->blocks_[z][0][x]            ->IsOpaque())) && // top
        ((z >= 1            && chunk.blocks_[z-1][y][x]->IsOpaque()) || (z == 0             && neighbors[kFront ]->blocks_[Chunk::SIZE-1][y][x]->IsOpaque())) && // front
        ((z < Chunk::SIZE-1 && chunk.blocks_[z+1][y][x]->IsOpaque()) || (z == Chunk::SIZE-1 && neighbors[kBack  ]->blocks_[0][y][x]            ->IsOpaque())))   // back
      {
        return;
      }

      auto&& vertex_data = block_type->GetVertexData();
      
      assert(vertex_data.size() % 8 == 0);
      for (int fi = 0; fi < vertex_data.size(); ) { // float index

        // position vertices
        vertex_buffer_data.push_back(vertex_data[fi++] + (GLfloat)x + chunk_offset.x);
        vertex_buffer_data.push_back(vertex_data[fi++] + (GLfloat)y + chunk_offset.y);
        vertex_buffer_data.push_back(vertex_data[fi++] + (GLfloat)z + chunk_offset.z);

        // normal vertices
        vertex_buffer_data.push_back(vertex_data[fi++]);
        vertex_buffer_data.push_back(vertex_data[fi++]);
        vertex_buffer_data.push_back(vertex_data[fi++]);

        // uv vertices
        vertex_buffer_data.push_back(vertex_data[fi++]);
        vertex_buffer_data.push_back(vertex_data[fi++]);
      }
    });

    
    chunk.vb_index_count_ = vertex_buffer_data.size();

    // copy vector to graphic card
    chunk.InitializeGL();

    glBindBuffer(GL_ARRAY_BUFFER, chunk.vb_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_buffer_data.size(),
                 vertex_buffer_data.data(), GL_STATIC_DRAW);

    glBindVertexArray(chunk.vao_);

    constexpr GLsizei stride = (3 + 3 + 2) * sizeof(GLfloat);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // uv attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    CHECK_GL();

    chunk.state_ = kViewPrepared;
  }

  void ChunkGeneratorBase::BindTexture() {
    block_registrar_->BindTexture();
  }
}
