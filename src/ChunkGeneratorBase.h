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

  class ChunkGeneratorBase : public JSObjectWrap<ChunkGeneratorBase> {
    JS_IMPL_WRAP(ChunkGeneratorBase, ONE_ARG({
      JS_SETTER(BlockRegistrar),
      JS_SETTER(WorldGenerator),
    }))

    JS_IMPL_SETTER_WRAP(BlockRegistrarBase, BlockRegistrar)
    JS_IMPL_SETTER_OBJECT(WorldGenerator)

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
