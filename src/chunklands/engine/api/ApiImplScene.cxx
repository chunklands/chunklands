
#include "api-shared.hxx"

namespace chunklands::engine {

  boost::future<void>
  Api::SceneAddChunk(CEChunkHandle* handle) {
    EASY_FUNCTION();
    API_FN();

    return EnqueueTask(executor_, [this, handle]() {
      EASY_FUNCTION();
      CHECK(has_handle(chunks_, handle));
      auto insert_result = scene_chunks_.insert(handle);
      CHECK(insert_result.second);
    });
  }

  boost::future<void>
  Api::SceneRemoveChunk(CEChunkHandle* handle) {
    EASY_FUNCTION();
    API_FN();

    return EnqueueTask(executor_, [this, handle]() {
      EASY_FUNCTION();
      const std::size_t count = scene_chunks_.erase(handle);
      CHECK(count == 1);
    });
  }

} // namespace chunklands::engine