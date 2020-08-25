
#include <chunklands/engine/Chunk.hxx>
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine/shared.hxx>

namespace chunklands::engine {

AsyncEngineResult<CEChunkHandle*>
Engine::ChunkCreate(int x, int y, int z)
{
    EASY_FUNCTION();
    ENGINE_FN();
    std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(x, y, z);

    return EnqueueTask(data_->executors.opengl, [this, chunk = std::move(chunk)]() mutable -> EngineResultX<CEChunkHandle*> {
        EASY_FUNCTION();

        assert(chunk->state == ChunkState::kEmpty);
        data_->chunk.chunks.insert(chunk.get());
        data_->chunk.by_state[ChunkState::kEmpty].insert(chunk.get());
        auto result_by_pos_insert = data_->chunk.by_pos.insert({ glm::ivec3(chunk->x, chunk->y, chunk->z), chunk.get() });
        ENGINE_CHECK_MSG(result_by_pos_insert.second, "chunk was not present in the map");

        return Ok(reinterpret_cast<CEChunkHandle*>(chunk.release()));
    });
}

AsyncEngineResult<CENone>
Engine::ChunkDelete(CEChunkHandle* handle)
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle]() -> EngineResultX<CENone> {
        EASY_FUNCTION();

        std::unique_ptr<Chunk> chunk;
        ENGINE_CHECK(get_handle(chunk, data_->chunk.chunks, handle));
        ENGINE_CHECK(has_handle(data_->chunk.by_state[chunk->state], handle));

        const std::size_t erased = data_->chunk.chunks.erase(chunk.get());
        assert(erased == 1);

        const std::size_t erased_by_state = data_->chunk.by_state[chunk->state].erase(chunk.get());
        assert(erased_by_state == 1);

        const std::size_t erased_by_pos = data_->chunk.by_pos.erase(glm::ivec3(chunk->x, chunk->y, chunk->z));
        assert(erased_by_pos == 1);

        // unique_ptr deletes chunk
        return Ok();
    });
}

AsyncEngineResult<CENone>
Engine::ChunkUpdateData(CEChunkHandle* handle, CEBlockHandle** blocks)
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle, blocks]() -> EngineResultX<CENone> {
        EASY_FUNCTION();
        Chunk* chunk = nullptr;
        ENGINE_CHECK(get_handle(&chunk, data_->chunk.chunks, handle));

        const std::size_t erase_count = data_->chunk.by_state[chunk->state].erase(chunk);
        assert(erase_count == 1);

        // std::memcpy(&chunk->blocks, blocks, sizeof(chunk->blocks));
        for (int z = 0, i = 0; z < CE_CHUNK_SIZE; z++) {
            for (int y = 0; y < CE_CHUNK_SIZE; y++) {
                for (int x = 0; x < CE_CHUNK_SIZE; x++, i++) {
                    CEBlockHandle* const block_handle = blocks[i];
                    ENGINE_CHECK(has_handle(data_->block.blocks, block_handle));
                    chunk->blocks[z][y][x] = reinterpret_cast<Block*>(block_handle);
                }
            }
        }

        chunk->state = ChunkState::kDataPrepared;
        auto insert_result = data_->chunk.by_state[ChunkState::kDataPrepared].insert(chunk);
        assert(insert_result.second);

        return Ok();
    });
}

} // namespace chunklands::engine