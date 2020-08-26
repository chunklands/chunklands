
#include <chunklands/engine/Chunk.hxx>
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine/shared.hxx>

namespace chunklands::engine {

AsyncEngineResult<CEChunkHandle*>
Engine::ChunkCreate(int x, int y, int z)
{
    EASY_FUNCTION();
    std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(x, y, z);

    return EnqueueTask(data_->executors.opengl, [this, chunk = std::move(chunk)]() mutable -> EngineResultX<CEChunkHandle*> {
        EASY_FUNCTION();

        data_->chunk.Add(chunk.get());
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

        Chunk* chunk = nullptr;
        ENGINE_CHECK(data_->chunk.GetChunk(&chunk, handle));
        std::unique_ptr<Chunk> ch(chunk);

        data_->chunk.Remove(chunk);
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
        ENGINE_CHECK(data_->chunk.GetChunk(&chunk, handle));

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

        data_->chunk.SetChunkDataPrepared(chunk);
        return Ok();
    });
}

} // namespace chunklands::engine