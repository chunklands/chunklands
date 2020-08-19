
#include "api-shared.hxx"
#include "../Chunk.hxx"

namespace chunklands::engine {

  boost::future<CEChunkHandle*>
  Api::ChunkCreate(int x, int y, int z) {
    EASY_FUNCTION();
    API_FN();
    std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(x, y, z);

    return EnqueueTask(executor_, [this, chunk = std::move(chunk)]() mutable {
      EASY_FUNCTION();
      CEChunkHandle* const handle = reinterpret_cast<CEChunkHandle*>(chunk.get());

      assert(chunk->state == ChunkState::kEmpty);
      chunks_.insert(handle);
      chunks_by_state_[ChunkState::kEmpty].insert(handle);
      auto result_by_pos_insert = data_->chunk.by_pos.insert({glm::ivec3(chunk->x, chunk->y, chunk->z), chunk.get()});
      CHECK(result_by_pos_insert.second);

      chunk.release();
      return handle;
    });
  }

  boost::future<void>
  Api::ChunkDelete(CEChunkHandle* handle) {
    EASY_FUNCTION();
    API_FN();

    return EnqueueTask(executor_, [this, handle]() {
      EASY_FUNCTION();
      
      CHECK(has_handle(chunks_, handle));
      std::unique_ptr<Chunk> chunk(reinterpret_cast<Chunk*>(handle));
      CHECK(has_handle(chunks_by_state_[chunk->state], handle));

      const std::size_t erased = chunks_.erase(handle);
      assert(erased == 1);

      const std::size_t erased_by_state = chunks_by_state_[chunk->state].erase(handle);
      assert(erased_by_state == 1);

      const std::size_t erased_by_pos = data_->chunk.by_pos.erase(glm::ivec3(chunk->x, chunk->y, chunk->z));
      assert(erased_by_pos == 1);

      // unique_ptr deletes chunk
    });
  }

  boost::future<void>
  Api::ChunkUpdateData(CEChunkHandle* handle, CEBlockHandle** blocks) {
    EASY_FUNCTION();
    API_FN();

    return EnqueueTask(executor_, [this, handle, blocks]() {
      EASY_FUNCTION();
      CHECK(has_handle(chunks_, handle));

      Chunk* chunk = reinterpret_cast<Chunk*>(handle);
      assert(chunk);

      const std::size_t erase_count = chunks_by_state_[chunk->state].erase(handle);
      assert(erase_count == 1);

      // std::memcpy(&chunk->blocks, blocks, sizeof(chunk->blocks));
      for (int z = 0, i = 0; z < CE_CHUNK_SIZE; z++) {
        for (int y = 0; y < CE_CHUNK_SIZE; y++) {
          for (int x = 0; x < CE_CHUNK_SIZE; x++, i++) {
            CEBlockHandle* block_handle = blocks[i];
            CHECK(has_handle(blocks_, block_handle));
            chunk->blocks[z][y][x] = reinterpret_cast<Block*>(block_handle);
          }
        }
      }

      chunk->state = ChunkState::kDataPrepared;
      auto insert_result = chunks_by_state_[ChunkState::kDataPrepared].insert(handle);
      assert(insert_result.second);
    });
  }

} // namespace chunklands::engine