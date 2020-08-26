#ifndef __CHUNKLANDS_ENGINE_CHUNKMESHDATAGENERATOR_HXX__
#define __CHUNKLANDS_ENGINE_CHUNKMESHDATAGENERATOR_HXX__

#include "Chunk.hxx"
#include "gl/Vao.hxx"
#include <boost/move/core.hpp>
#include <cassert>

namespace chunklands::engine {

struct ChunkMeshDataGenerator {
    BOOST_MOVABLE_BUT_NOT_COPYABLE(ChunkMeshDataGenerator)
public:
    ChunkMeshDataGenerator(Chunk* chunk)
        : chunk(chunk)
    {
        assert(chunk != nullptr);
    }

    void operator()();

public:
    Chunk* const chunk;
};

} // namespace chunklands::engine

#endif