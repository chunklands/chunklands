#ifndef __CHUNKLANDS_CHUNKGENERATORBASE_H__
#define __CHUNKLANDS_CHUNKGENERATORBASE_H__

#include <queue>
#include <napi.h>
#include "BlockRegistrarBase.h"
#include "Chunk.h"
#include "napi/object_wrap_util.h"
#include "napi/PersistentObjectWrap.h"

namespace chunklands {
  namespace detail {
    struct loaded_chunks_data {
      std::shared_ptr<Chunk> chunk;
      Napi::ObjectReference js_chunk_data;
    };
  }

  class ChunkGeneratorBase : public Napi::ObjectWrap<ChunkGeneratorBase> {
    DECLARE_OBJECT_WRAP(ChunkGeneratorBase)
    DECLARE_OBJECT_WRAP_CB(void SetBlockRegistrar)
    DECLARE_OBJECT_WRAP_CB(void SetWorldGenerator)

  public:
    void GenerateModel(std::shared_ptr<Chunk>& chunk);
    bool ProcessNextModel();
    void GenerateView(Chunk& chunk, const Chunk* neighbors[kNeighborCount]);

    void BindTexture();

  private:
    NapiExt::PersistentObjectWrap<BlockRegistrarBase> block_registrar_;
    Napi::ObjectReference world_generator_;

    std::queue<detail::loaded_chunks_data> loaded_chunks_;
  };
}

#endif
