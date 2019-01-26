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

  void ChunkGeneratorBase::Generate(Chunk& chunk) {
    assert(chunk.GetState() == kEmpty);
    auto&& pos = chunk.GetPos();

    chunk.ForEachBlock([&](char& block_type, int x, int y, int z) {
      glm::ivec3 abs_pos((int)Chunk::SIZE * pos + glm::ivec3(x, y, z));
      block_type = IsGroundMountains(abs_pos) ? 1 : 0;
    });
  }
}
