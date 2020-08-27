#ifndef __CHUNKLANDS_ENGINE_SCENE_CHUNK_HXX__
#define __CHUNKLANDS_ENGINE_SCENE_CHUNK_HXX__

#include <chunklands/engine/Block.hxx>
#include <chunklands/engine/GBufferMesh.hxx>
#include <chunklands/engine/engine_types.hxx>
#include <cstring>

namespace chunklands::engine {

enum ChunkState {
    kEmpty,
    kDataPrepared,
    kDataPreparedWithAllNeighbors,
    kMeshPrepared,
    kCount,
};

enum ChunkNeighbor {
    kChunkNeighborLeft,
    kChunkNeighborRight,
    kChunkNeighborBottom,
    kChunkNeighborTop,
    kChunkNeighborFront,
    kChunkNeighborBack,
    kChunkNeighborCount
};

struct Chunk {
    Chunk(int x, int y, int z)
        : x(x)
        , y(y)
        , z(z)
    {
        std::memset(&blocks, 0, sizeof(blocks));
        neighbors[kChunkNeighborLeft] = nullptr;
        neighbors[kChunkNeighborRight] = nullptr;
        neighbors[kChunkNeighborBottom] = nullptr;
        neighbors[kChunkNeighborTop] = nullptr;
        neighbors[kChunkNeighborFront] = nullptr;
        neighbors[kChunkNeighborBack] = nullptr;
    }

    bool HasAllNeighborsDataPrepared() const
    {
        return state >= kDataPrepared
            && (neighbors[kChunkNeighborLeft] != nullptr && neighbors[kChunkNeighborLeft]->state >= kDataPrepared)
            && (neighbors[kChunkNeighborRight] != nullptr && neighbors[kChunkNeighborRight]->state >= kDataPrepared)
            && (neighbors[kChunkNeighborBottom] != nullptr && neighbors[kChunkNeighborBottom]->state >= kDataPrepared)
            && (neighbors[kChunkNeighborTop] != nullptr && neighbors[kChunkNeighborTop]->state >= kDataPrepared)
            && (neighbors[kChunkNeighborFront] != nullptr && neighbors[kChunkNeighborFront]->state >= kDataPrepared)
            && (neighbors[kChunkNeighborBack] != nullptr && neighbors[kChunkNeighborBack]->state >= kDataPrepared);
    }

    int x, y, z;
    ChunkState state = ChunkState::kEmpty;
    Block* blocks[CE_CHUNK_SIZE][CE_CHUNK_SIZE][CE_CHUNK_SIZE];
    Chunk* neighbors[kChunkNeighborCount];

    GBufferMesh mesh;
    double mesh_time = 0.0;
};

} // namespace chunklands::engine

#endif