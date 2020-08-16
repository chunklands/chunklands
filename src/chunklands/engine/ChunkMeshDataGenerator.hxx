#ifndef __CHUNKLANDS_ENGINE_CHUNKMESHDATAGENERATOR_HXX__
#define __CHUNKLANDS_ENGINE_CHUNKMESHDATAGENERATOR_HXX__

#include <cassert>
#include <boost/move/core.hpp>
#include "Chunk.hxx"
#include "gl/Vao.hxx"

namespace chunklands::engine {

  enum ChunkNeighbor {
    kChunkNeighborLeft,
    kChunkNeighborRight,
    kChunkNeighborBottom,
    kChunkNeighborTop,
    kChunkNeighborFront,
    kChunkNeighborBack,
    kChunkNeighborCount
  };

  struct ChunkMeshDataGenerator {
    BOOST_MOVABLE_BUT_NOT_COPYABLE(ChunkMeshDataGenerator)
  public:
    ChunkMeshDataGenerator(Chunk* chunk, std::array<const Chunk*, kChunkNeighborCount> neighbors) : chunk(chunk), neighbors(std::move(neighbors)) {
      assert(chunk != nullptr);

      for (const Chunk* neighbor : this->neighbors) {
        assert(neighbor->state >= kDataPrepared);
      }
    }

    void operator()();

  public:
    Chunk* const chunk;
    std::array<const Chunk*, kChunkNeighborCount> neighbors;
  };

} // namespace chunklands::engine

#endif