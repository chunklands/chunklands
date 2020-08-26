
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine/shared.hxx>

namespace chunklands::engine {

AsyncEngineResult<CENone>
Engine::SceneAddChunk(CEChunkHandle* handle)
{
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle]() -> EngineResultX<CENone> {
        EASY_FUNCTION();
        Chunk* chunk = nullptr;
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
        Chunk* chunk = nullptr;
        ENGINE_CHECK(data_->chunk.GetChunk(&chunk, handle));
        ENGINE_CHECK_MSG(data_->chunk.RemoveFromScene(chunk), "remove chunk from scene failed");

        return Ok();
    });
}

} // namespace chunklands::engine