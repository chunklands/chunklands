
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine/shared.hxx>

namespace chunklands::engine {

  boost::future<void>
  Engine::SceneAddChunk(CEChunkHandle* handle) {
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle]() {
      EASY_FUNCTION();
      CHECK_OR_FATAL(has_handle(data_->chunk.chunks, handle));
      Chunk* const chunk = reinterpret_cast<Chunk*>(handle);
      auto insert_result = data_->chunk.scene.insert(chunk);
      CHECK_OR_FATAL(insert_result.second);
    });
  }

  boost::future<void>
  Engine::SceneRemoveChunk(CEChunkHandle* handle) {
    EASY_FUNCTION();
    ENGINE_FN();

    return EnqueueTask(data_->executors.opengl, [this, handle]() {
      EASY_FUNCTION();
      CHECK_OR_FATAL(has_handle(data_->chunk.chunks, handle));
      Chunk* const chunk = reinterpret_cast<Chunk*>(handle);
      const std::size_t count = data_->chunk.scene.erase(chunk);
      CHECK_OR_FATAL(count == 1);
    });
  }

} // namespace chunklands::engine