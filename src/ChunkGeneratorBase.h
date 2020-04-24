#ifndef __CHUNKLANDS_CHUNKGENERATORBASE_H__
#define __CHUNKLANDS_CHUNKGENERATORBASE_H__

#include <queue>
#include "BlockRegistrarBase.h"
#include "Chunk.h"
#include "js.h"

namespace chunklands {
  namespace detail {
    struct loaded_chunks_data {
      std::shared_ptr<Chunk> chunk;
      Napi::ObjectReference js_chunk_data;
    };
  }

  class ChunkGeneratorBase : public JSWrap<ChunkGeneratorBase> {
    JS_DECL_WRAP(ChunkGeneratorBase)
    JS_DECL_SETTER_REF(BlockRegistrarBase, BlockRegistrar)
    JS_DECL_SETTER_OBJ(WorldGenerator)

  public:
    void GenerateModel(std::shared_ptr<Chunk>& chunk);
    bool ProcessNextModel();
    void GenerateView(Chunk& chunk, const Chunk* neighbors[kNeighborCount]);

    void BindTexture();

  private:

    std::queue<detail::loaded_chunks_data> loaded_chunks_;
  };
}

#endif
