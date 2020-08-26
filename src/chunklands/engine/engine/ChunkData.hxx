#ifndef __CHUNKLANDS_ENGINE_ENGINE_CHUNKDATA_HXX__
#define __CHUNKLANDS_ENGINE_ENGINE_CHUNKDATA_HXX__

#include <boost/container_hash/hash.hpp>
#include <chunklands/engine/Chunk.hxx>
#include <chunklands/engine/ChunkMeshDataGenerator.hxx>
#include <chunklands/engine/engine/handle.hxx>
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

struct ChunkData {
public:
    bool GetChunk(Chunk** chunk, CEChunkHandle* handle)
    {
        return get_handle(chunk, chunks_, handle);
    }

    void Add(Chunk* chunk);
    void Remove(Chunk* chunk);

    void SetChunkDataPrepared(Chunk* chunk);
    bool AddToScene(Chunk* chunk)
    {
        return scene_.insert(chunk).second == 1;
    }
    bool RemoveFromScene(Chunk* chunk)
    {
        return scene_.erase(chunk) == 1;
    }

    void __IntegrityCheck() const;

    const std::set<Chunk*>& Chunks() const { return chunks_; }

    const std::set<Chunk*>& EmptyChunks() const
    {
        return by_state_[ChunkState::kEmpty];
    }

    const std::set<Chunk*>& DataPreparedChunks() const
    {
        return by_state_[ChunkState::kDataPrepared];
    }

    const std::set<Chunk*>& MeshPreparedChunks() const
    {
        return by_state_[ChunkState::kMeshPrepared];
    }

    const std::set<Chunk*>& SceneChunks() const { return scene_; }

    void UpdateChunks();

private:
    std::set<Chunk*> chunks_;
    std::set<Chunk*> by_state_[ChunkState::kCount];
    std::set<Chunk*> scene_;
    std::unordered_map<glm::ivec3, Chunk*, ivec3_hasher> by_pos_;
};

} // namespace chunklands::engine

#endif