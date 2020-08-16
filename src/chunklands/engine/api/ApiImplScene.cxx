
#include "api-shared.hxx"

namespace chunklands::engine {

  boost::future<void>
  Api::SceneAddChunk(CEChunkHandle* handle) {
    EASY_FUNCTION();
    API_FN();
    CHECK(has_handle(chunks_, handle));
    CHECK(!has_handle(scene_chunks_, handle));

    return EnqueueTask(executor_, [this, handle]() {
      EASY_FUNCTION();
      scene_chunks_.insert(handle);

      // CEGBufferMeshHandle* const mesh_handle = reinterpret_cast<CEGBufferMeshHandle*>(mesh.get());
      // g_buffer_meshes_.insert(mesh_handle);
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