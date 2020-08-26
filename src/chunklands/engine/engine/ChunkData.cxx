
#include "ChunkData.hxx"

#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/easylogging++.hxx>

namespace chunklands::engine {

void ChunkData::Add(Chunk* chunk)
{
    LOG(DEBUG) << "chunk " << chunk << " add";
    auto insert_chunks_result = chunks_.insert(chunk);
    assert(insert_chunks_result.second);

    assert(chunk->state == ChunkState::kEmpty);
    auto insert_by_state_result = by_state_[ChunkState::kEmpty].insert(chunk);
    assert(insert_by_state_result.second);

    auto insert_by_pos_result = by_pos_.insert({ glm::ivec3(chunk->x, chunk->y, chunk->z), chunk });
    assert(insert_by_pos_result.second);

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x - 1, chunk->y, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborLeft] == nullptr);
            chunk->neighbors[kChunkNeighborLeft] = it->second;

            assert(it->second->neighbors[kChunkNeighborRight] == nullptr);
            it->second->neighbors[kChunkNeighborRight] = chunk;

            LOG(DEBUG) << "chunk " << chunk << " set left neighbor " << it->second;
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x + 1, chunk->y, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborRight] == nullptr);
            chunk->neighbors[kChunkNeighborRight] = it->second;

            assert(it->second->neighbors[kChunkNeighborLeft] == nullptr);
            it->second->neighbors[kChunkNeighborLeft] = chunk;

            LOG(DEBUG) << "chunk " << chunk << " set right neighbor " << it->second;
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y - 1, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborBottom] == nullptr);
            chunk->neighbors[kChunkNeighborBottom] = it->second;

            assert(it->second->neighbors[kChunkNeighborTop] == nullptr);
            it->second->neighbors[kChunkNeighborTop] = chunk;

            LOG(DEBUG) << "chunk " << chunk << " set bottom neighbor " << it->second;
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y + 1, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborTop] == nullptr);
            chunk->neighbors[kChunkNeighborTop] = it->second;

            assert(it->second->neighbors[kChunkNeighborBottom] == nullptr);
            it->second->neighbors[kChunkNeighborBottom] = chunk;

            LOG(DEBUG) << "chunk " << chunk << " set top neighbor " << it->second;
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y, chunk->z - 1));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborFront] == nullptr);
            chunk->neighbors[kChunkNeighborFront] = it->second;

            assert(it->second->neighbors[kChunkNeighborBack] == nullptr);
            it->second->neighbors[kChunkNeighborBack] = chunk;

            LOG(DEBUG) << "chunk " << chunk << " set front neighbor " << it->second;
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y, chunk->z + 1));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborBack] == nullptr);
            chunk->neighbors[kChunkNeighborBack] = it->second;

            assert(it->second->neighbors[kChunkNeighborFront] == nullptr);
            it->second->neighbors[kChunkNeighborFront] = chunk;

            LOG(DEBUG) << "chunk " << chunk << " set back neighbor " << it->second;
        }
    }

    __IntegrityCheck();
}

void ChunkData::SetChunkDataPrepared(Chunk* chunk)
{
    LOG(DEBUG) << "chunk " << chunk << " set data prepared";

    assert(has_handle(chunks_, chunk));
    size_t erase_result = by_state_[chunk->state].erase(chunk);
    assert(erase_result == 1);

    if (chunk->HasAllNeighborsDataPrepared()) {
        chunk->state = kDataPreparedWithAllNeighbors;
        by_state_[kDataPreparedWithAllNeighbors].insert(chunk);

        LOG(DEBUG) << "chunk " << chunk << " has all neighbors' data prepared, add to kDataPreparedWithAllNeighbors";
    } else {
        chunk->state = kDataPrepared;
        by_state_[kDataPrepared].insert(chunk);

        LOG(DEBUG) << "chunk " << chunk << " has not all neighbors' data prepared, add to kDataPrepared";
    }

    for (int i = 0; i < kChunkNeighborCount; i++) {
        Chunk* const neighbor = chunk->neighbors[i];
        if (neighbor == nullptr) {
            continue;
        }

        if (!neighbor->HasAllNeighborsDataPrepared()) {
            continue;
        }

        LOG(DEBUG) << "chunk " << chunk << ", neighbor " << neighbor << " erase from " << neighbor->state;
        size_t erase_result = by_state_[neighbor->state].erase(neighbor);
        assert(erase_result == 1);

        neighbor->state = kDataPreparedWithAllNeighbors;

        by_state_[kDataPreparedWithAllNeighbors].insert(neighbor);
        LOG(DEBUG) << "chunk " << chunk << ", neighbor " << neighbor << " add to " << kDataPreparedWithAllNeighbors;
    }

    __IntegrityCheck();
}

void ChunkData::Remove(Chunk* chunk)
{
    EASY_FUNCTION();
    LOG(DEBUG) << "chunk " << chunk << " remove";

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
            assert(chunk->neighbors[kChunkNeighborLeft] == it->second);
            chunk->neighbors[kChunkNeighborLeft] = nullptr;

            assert(it->second->neighbors[kChunkNeighborRight] == chunk);
            it->second->neighbors[kChunkNeighborRight] = nullptr;

            if (it->second->state == kDataPreparedWithAllNeighbors) {
                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = kDataPrepared;

                auto insert_result = by_state_[kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << kDataPrepared;
            }
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x + 1, chunk->y, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborRight] == it->second);
            chunk->neighbors[kChunkNeighborRight] = nullptr;

            assert(it->second->neighbors[kChunkNeighborLeft] == chunk);
            it->second->neighbors[kChunkNeighborLeft] = nullptr;

            if (it->second->state == kDataPreparedWithAllNeighbors) {
                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = kDataPrepared;

                auto insert_result = by_state_[kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << kDataPrepared;
            }
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y - 1, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborBottom] == it->second);
            chunk->neighbors[kChunkNeighborBottom] = nullptr;

            assert(it->second->neighbors[kChunkNeighborTop] == chunk);
            it->second->neighbors[kChunkNeighborTop] = nullptr;

            if (it->second->state == kDataPreparedWithAllNeighbors) {
                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = kDataPrepared;

                auto insert_result = by_state_[kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << kDataPrepared;
            }
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y + 1, chunk->z));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborTop] == it->second);
            chunk->neighbors[kChunkNeighborTop] = nullptr;

            assert(it->second->neighbors[kChunkNeighborBottom] == chunk);
            it->second->neighbors[kChunkNeighborBottom] = nullptr;

            if (it->second->state == kDataPreparedWithAllNeighbors) {
                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = kDataPrepared;

                auto insert_result = by_state_[kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << kDataPrepared;
            }
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y, chunk->z - 1));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborFront] == it->second);
            chunk->neighbors[kChunkNeighborFront] = nullptr;

            assert(it->second->neighbors[kChunkNeighborBack] == chunk);
            it->second->neighbors[kChunkNeighborBack] = nullptr;

            if (it->second->state == kDataPreparedWithAllNeighbors) {
                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = kDataPrepared;

                auto insert_result = by_state_[kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << kDataPrepared;
            }
        }
    }

    {
        auto it = by_pos_.find(glm::ivec3(chunk->x, chunk->y, chunk->z + 1));
        if (it != by_pos_.end()) {
            assert(chunk->neighbors[kChunkNeighborBack] == it->second);
            chunk->neighbors[kChunkNeighborBack] = nullptr;

            assert(it->second->neighbors[kChunkNeighborFront] == chunk);
            it->second->neighbors[kChunkNeighborFront] = nullptr;

            if (it->second->state == kDataPreparedWithAllNeighbors) {
                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " remove from " << kDataPreparedWithAllNeighbors;
                size_t erase_result = by_state_[kDataPreparedWithAllNeighbors].erase(it->second);
                assert(erase_result == 1);

                it->second->state = kDataPrepared;

                auto insert_result = by_state_[kDataPrepared].insert(it->second);
                assert(insert_result.second);

                LOG(DEBUG) << "chunk " << chunk << ", neighbor " << it->second << " add to " << kDataPrepared;
            }
        }
    }

    __IntegrityCheck();
}

void ChunkData::UpdateChunks()
{
    EASY_FUNCTION();
    constexpr int MAX_GENERATED = 1;

    LOG(DEBUG) << "chunks update";

    int generated = 0;
    for (auto it = by_state_[kDataPreparedWithAllNeighbors].begin(); it != by_state_[kDataPreparedWithAllNeighbors].end();) {

        Chunk* const chunk = *it;
        assert(chunk->state == kDataPreparedWithAllNeighbors);

        LOG(DEBUG) << "chunk " << chunk << " generate";

        generated++;
        ChunkMeshDataGenerator generator(chunk);
        generator();

        LOG(DEBUG) << "chunk " << chunk << " generate, remove from " << kDataPreparedWithAllNeighbors << " add to " << kMeshPrepared;
        it = by_state_[kDataPreparedWithAllNeighbors].erase(it);

        chunk->state = ChunkState::kMeshPrepared;
        auto insert_result = by_state_[ChunkState::kMeshPrepared].insert(chunk);
        assert(insert_result.second);

        if (generated >= MAX_GENERATED) {
            break;
        }
    }

    __IntegrityCheck();
}

void ChunkData::__IntegrityCheck() const
{
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
}

} // namespace chunklands::engine