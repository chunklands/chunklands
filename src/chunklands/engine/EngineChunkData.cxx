
#include "EngineChunkData.hxx"
#include "api_util.hxx"
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/easylogging++.hxx>

namespace chunklands::engine {

constexpr bool log = false;

void ChunkData::Add(chunk::Chunk* chunk)
{
    LOG_IF(log, DEBUG) << "chunk " << chunk << " add";
    auto insert_chunks_result = chunks_.insert(chunk);
    assert(insert_chunks_result.second);

    assert(chunk->state == chunk::ChunkState::kEmpty);
    auto insert_by_state_result = by_state_[chunk::ChunkState::kEmpty].insert(chunk);
    assert(insert_by_state_result.second);

    auto insert_by_pos_result = by_pos_.insert({ glm::ivec3(chunk->x, chunk->y, chunk->z), chunk });
    assert(insert_by_pos_result.second);

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x - 1, chunk->y, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborLeft] == nullptr);
            chunk->neighbors[chunk::kChunkNeighborLeft] = it->second;

            assert(it->second->neighbors[chunk::kChunkNeighborRight] == nullptr);
            it->second->neighbors[chunk::kChunkNeighborRight] = chunk;

            LOG_IF(log, DEBUG) << "chunk " << chunk << " set left neighbor " << it->second;
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x + 1, chunk->y, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborRight] == nullptr);
            chunk->neighbors[chunk::kChunkNeighborRight] = it->second;

            assert(it->second->neighbors[chunk::kChunkNeighborLeft] == nullptr);
            it->second->neighbors[chunk::kChunkNeighborLeft] = chunk;

            LOG_IF(log, DEBUG) << "chunk " << chunk << " set right neighbor " << it->second;
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y - 1, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborBottom] == nullptr);
            chunk->neighbors[chunk::kChunkNeighborBottom] = it->second;

            assert(it->second->neighbors[chunk::kChunkNeighborTop] == nullptr);
            it->second->neighbors[chunk::kChunkNeighborTop] = chunk;

            LOG_IF(log, DEBUG) << "chunk " << chunk << " set bottom neighbor " << it->second;
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y + 1, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborTop] == nullptr);
            chunk->neighbors[chunk::kChunkNeighborTop] = it->second;

            assert(it->second->neighbors[chunk::kChunkNeighborBottom] == nullptr);
            it->second->neighbors[chunk::kChunkNeighborBottom] = chunk;

            LOG_IF(log, DEBUG) << "chunk " << chunk << " set top neighbor " << it->second;
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y, chunk->z - 1));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborFront] == nullptr);
            chunk->neighbors[chunk::kChunkNeighborFront] = it->second;

            assert(it->second->neighbors[chunk::kChunkNeighborBack] == nullptr);
            it->second->neighbors[chunk::kChunkNeighborBack] = chunk;

            LOG_IF(log, DEBUG) << "chunk " << chunk << " set front neighbor " << it->second;
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y, chunk->z + 1));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborBack] == nullptr);
            chunk->neighbors[chunk::kChunkNeighborBack] = it->second;

            assert(it->second->neighbors[chunk::kChunkNeighborFront] == nullptr);
            it->second->neighbors[chunk::kChunkNeighborFront] = chunk;

            LOG_IF(log, DEBUG) << "chunk " << chunk << " set back neighbor " << it->second;
        }
    }

    __IntegrityCheck();
}

void ChunkData::SetChunkDataPrepared(chunk::Chunk* chunk)
{
    LOG_IF(log, DEBUG) << "chunk " << chunk << " set data prepared";

    assert(has_handle(chunks_, chunk));
    size_t erase_result = by_state_[chunk->state].erase(chunk);
    assert(erase_result == 1);

    if (chunk->HasAllNeighborsDataPrepared()) {
        chunk->state = chunk::kDataPreparedWithAllNeighbors;
        by_state_[chunk::kDataPreparedWithAllNeighbors].insert(chunk);

        LOG_IF(log, DEBUG) << "chunk " << chunk << " has all neighbors' data prepared, add to kDataPreparedWithAllNeighbors";
    } else {
        chunk->state = chunk::kDataPrepared;
        by_state_[chunk::kDataPrepared].insert(chunk);

        LOG_IF(log, DEBUG) << "chunk " << chunk << " has not all neighbors' data prepared, add to kDataPrepared";
    }

    for (int i = 0; i < chunk::kChunkNeighborCount; i++) {
        chunk::Chunk* const neighbor = chunk->neighbors[i];
        if (neighbor == nullptr) {
            continue;
        }

        if (neighbor->state >= chunk::kMeshPrepared) {
            continue;
        }

        if (!neighbor->HasAllNeighborsDataPrepared()) {
            continue;
        }

        LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << neighbor << " erase from " << neighbor->state;
        size_t erase_result = by_state_[neighbor->state].erase(neighbor);
        assert(erase_result == 1);

        neighbor->state = chunk::kDataPreparedWithAllNeighbors;

        by_state_[chunk::kDataPreparedWithAllNeighbors].insert(neighbor);
        LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << neighbor << " add to " << chunk::kDataPreparedWithAllNeighbors;
    }

    __IntegrityCheck();
}

void ChunkData::Remove(chunk::Chunk* chunk)
{
    EASY_FUNCTION();
    LOG_IF(log, DEBUG) << "chunk " << chunk << " remove";

    size_t erase_chunk_result = chunks_.erase(chunk);
    assert(erase_chunk_result == 1);

    size_t erase_by_state_result = by_state_[chunk->state].erase(chunk);
    assert(erase_by_state_result == 1);

    size_t erase_by_pos_result = by_pos_.erase(glm::ivec3(chunk->x, chunk->y, chunk->z));
    assert(erase_by_pos_result == 1);

    scene_.erase(chunk);

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x - 1, chunk->y, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborLeft] == it->second);
            chunk->neighbors[chunk::kChunkNeighborLeft] = nullptr;

            assert(it->second->neighbors[chunk::kChunkNeighborRight] == chunk);
            it->second->neighbors[chunk::kChunkNeighborRight] = nullptr;

            if (it->second->state == chunk::kDataPreparedWithAllNeighbors) {
                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << chunk::kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[chunk::kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = chunk::kDataPrepared;

                auto insert_result = by_state_[chunk::kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << chunk::kDataPrepared;
            }
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x + 1, chunk->y, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborRight] == it->second);
            chunk->neighbors[chunk::kChunkNeighborRight] = nullptr;

            assert(it->second->neighbors[chunk::kChunkNeighborLeft] == chunk);
            it->second->neighbors[chunk::kChunkNeighborLeft] = nullptr;

            if (it->second->state == chunk::kDataPreparedWithAllNeighbors) {
                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << chunk::kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[chunk::kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = chunk::kDataPrepared;

                auto insert_result = by_state_[chunk::kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << chunk::kDataPrepared;
            }
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y - 1, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborBottom] == it->second);
            chunk->neighbors[chunk::kChunkNeighborBottom] = nullptr;

            assert(it->second->neighbors[chunk::kChunkNeighborTop] == chunk);
            it->second->neighbors[chunk::kChunkNeighborTop] = nullptr;

            if (it->second->state == chunk::kDataPreparedWithAllNeighbors) {
                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << chunk::kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[chunk::kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = chunk::kDataPrepared;

                auto insert_result = by_state_[chunk::kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << chunk::kDataPrepared;
            }
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y + 1, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborTop] == it->second);
            chunk->neighbors[chunk::kChunkNeighborTop] = nullptr;

            assert(it->second->neighbors[chunk::kChunkNeighborBottom] == chunk);
            it->second->neighbors[chunk::kChunkNeighborBottom] = nullptr;

            if (it->second->state == chunk::kDataPreparedWithAllNeighbors) {
                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << chunk::kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[chunk::kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = chunk::kDataPrepared;

                auto insert_result = by_state_[chunk::kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << chunk::kDataPrepared;
            }
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y, chunk->z - 1));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborFront] == it->second);
            chunk->neighbors[chunk::kChunkNeighborFront] = nullptr;

            assert(it->second->neighbors[chunk::kChunkNeighborBack] == chunk);
            it->second->neighbors[chunk::kChunkNeighborBack] = nullptr;

            if (it->second->state == chunk::kDataPreparedWithAllNeighbors) {
                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << chunk::kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[chunk::kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = chunk::kDataPrepared;

                auto insert_result = by_state_[chunk::kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << chunk::kDataPrepared;
            }
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y, chunk->z + 1));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[chunk::kChunkNeighborBack] == it->second);
            chunk->neighbors[chunk::kChunkNeighborBack] = nullptr;

            assert(it->second->neighbors[chunk::kChunkNeighborFront] == chunk);
            it->second->neighbors[chunk::kChunkNeighborFront] = nullptr;

            if (it->second->state == chunk::kDataPreparedWithAllNeighbors) {
                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << chunk::kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[chunk::kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = chunk::kDataPrepared;

                auto insert_result = by_state_[chunk::kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG_IF(log, DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << chunk::kDataPrepared;
            }
        }
    }

    __IntegrityCheck();
}

void ChunkData::UpdateChunks()
{
    EASY_FUNCTION();
    constexpr int MAX_GENERATED = 1;

    LOG_IF(log, DEBUG) << "chunks update";

    int generated = 0;
    for (auto it = by_state_[chunk::kDataPreparedWithAllNeighbors].begin(); it != by_state_[chunk::kDataPreparedWithAllNeighbors].end();) {

        chunk::Chunk* const chunk = *it;
        assert(chunk->state == chunk::kDataPreparedWithAllNeighbors);

        generated++;
        chunk::ChunkMeshDataGenerator generator(chunk);
        generator();

        LOG_IF(log, DEBUG) << "chunk " << chunk << " generate, remove from " << chunk::kDataPreparedWithAllNeighbors << " add to " << chunk::kMeshPrepared;
        it = by_state_[chunk::kDataPreparedWithAllNeighbors].erase(it);

        chunk->state = chunk::ChunkState::kMeshPrepared;
        auto insert_result = by_state_[chunk::ChunkState::kMeshPrepared].insert(chunk);
        assert(insert_result.second);

        if (generated >= MAX_GENERATED) {
            break;
        }
    }

    __IntegrityCheck();
}

void ChunkData::__IntegrityCheck() const
{
#ifdef CHUNKLANDS_ENGINE_ENGINE_CHUNKDATA_INTEGRITY_CHECK
    for (Chunk* chunk : chunks_) {
        for (int i = 0; i < ChunkState::kCount; i++) {
            if (chunk->state == i) {
                assert(by_state_[i].find(chunk) != by_state_[i].end());
            } else {
                assert(by_state_[i].find(chunk) == by_state_[i].end());
            }
        }
    }

    for (int i = 0; i < ChunkState::kCount; i++) {
        for (Chunk* chunk : by_state_[i]) {
            assert(chunks_.find(chunk) != chunks_.end());

            for (int j = 0; j < ChunkState::kCount; j++) {
                if (i != j) {
                    assert(by_state_[j].find(chunk) == by_state_[j].end());
                }
            }
        }
    }
#endif
}

} // namespace chunklands::engine