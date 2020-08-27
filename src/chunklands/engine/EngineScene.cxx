
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"
#include <chunklands/libcxx/easy_profiler.hxx>

namespace chunklands::engine {

AsyncEngineResult<CENone>
Engine::SceneAddChunk(CEChunkHandle* handle)
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle]() -> EngineResultX<CENone> {
        EASY_FUNCTION();
        chunk::Chunk* chunk = nullptr;
        ENGINE_CHECK(data_->chunk.GetChunk(&chunk, handle));

        ENGINE_CHECK_MSG(data_->chunk.AddToScene(chunk), "adding chunk to scene failed");
        return Ok();
    });
}

AsyncEngineResult<CENone>
Engine::SceneRemoveChunk(CEChunkHandle* handle)
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle]() -> EngineResultX<CENone> {
        EASY_FUNCTION();
        chunk::Chunk* chunk = nullptr;
        ENGINE_CHECK(data_->chunk.GetChunk(&chunk, handle));
        ENGINE_CHECK_MSG(data_->chunk.RemoveFromScene(chunk), "remove chunk from scene failed");

        return Ok();
    });
}

} // namespace chunklands::engine