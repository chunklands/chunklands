#ifndef __CHUNKLANDS_ENGINE_ENGINECHUNKDATA_HXX__
#define __CHUNKLANDS_ENGINE_ENGINECHUNKDATA_HXX__

#include "handle.hxx"
#include <boost/container_hash/hash.hpp>
#include <chunklands/engine/chunk/Chunk.hxx>
#include <chunklands/engine/chunk/ChunkMeshDataGenerator.hxx>
#include <glm/vec3.hpp>
#include <set>

namespace chunklands::engine {

struct ivec3_hasher {
    std::size_t operator()(const glm::ivec3& v) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, boost::hash_value(v.x));
        boost::hash_combine(seed, boost::hash_value(v.y));
        boost::hash_combine(seed, boost::hash_value(v.z));

        return seed;
    }
};

struct EngineChunkData {
public:
    bool GetChunk(chunk::Chunk** chunk, CEChunkHandle* handle)
    {
        return get_handle(chunk, chunks_, handle);
    }

    void Add(chunk::Chunk* chunk);
    void Remove(chunk::Chunk* chunk);

    void SetChunkDataPrepared(chunk::Chunk* chunk);
    bool AddToScene(chunk::Chunk* chunk)
    {
        return scene_.insert(chunk).second == 1;
    }
    bool RemoveFromScene(chunk::Chunk* chunk)
    {
        return scene_.erase(chunk) == 1;
    }

    void __IntegrityCheck() const;

    const std::set<chunk::Chunk*>& Chunks() const { return chunks_; }

    const std::set<chunk::Chunk*>& EmptyChunks() const
    {
        return by_state_[chunk::ChunkState::kEmpty];
    }

    const std::set<chunk::Chunk*>& DataPreparedChunks() const
    {
        return by_state_[chunk::ChunkState::kDataPrepared];
    }

    const std::set<chunk::Chunk*>& MeshPreparedChunks() const
    {
        return by_state_[chunk::ChunkState::kMeshPrepared];
    }

    const std::set<chunk::Chunk*>& SceneChunks() const { return scene_; }

    void UpdateChunks();

    const std::unordered_map<glm::ivec3, chunk::Chunk*, ivec3_hasher>& GetChunksByPos() const
    {
        return by_pos_;
    }

private:
    std::set<chunk::Chunk*> chunks_;
    std::set<chunk::Chunk*> by_state_[chunk::ChunkState::kCount];
    std::set<chunk::Chunk*> scene_;
    std::unordered_map<glm::ivec3, chunk::Chunk*, ivec3_hasher> by_pos_;
};

} // namespace chunklands::engine

#endif