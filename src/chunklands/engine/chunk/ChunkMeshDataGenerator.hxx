#ifndef __CHUNKLANDS_ENGINE_CHUNK_CHUNKMESHDATAGENERATOR_HXX__
#define __CHUNKLANDS_ENGINE_CHUNK_CHUNKMESHDATAGENERATOR_HXX__

#include "Chunk.hxx"
#include <boost/move/core.hpp>
#include <cassert>

namespace chunklands::engine::chunk {

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

} // namespace chunklands::engine::chunk

#endif