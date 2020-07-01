#ifndef __CHUNKLANDS_GAME_CHUNKGENERATOR_HXX__
#define __CHUNKLANDS_GAME_CHUNKGENERATOR_HXX__

#include <chunklands/js.hxx>
#include <memory>
#include "Chunk.hxx"
#include "BlockRegistrar.hxx"
#include <queue>

namespace chunklands::game {

  namespace detail {
    struct loaded_chunks_data {
      std::shared_ptr<Chunk> chunk;
      JSRef<JSInt32Array> js_chunk_data;
    };
  }

  class ChunkGenerator : public JSObjectWrap<ChunkGenerator> {
    JS_IMPL_WRAP(ChunkGenerator, ONE_ARG({
      JS_SETTER(BlockRegistrar),
      JS_SETTER(WorldGenerator),
    }))

    JS_IMPL_SETTER_WRAP(BlockRegistrar, BlockRegistrar)
    JS_IMPL_SETTER_OBJECT(WorldGenerator)

  public:
    void GenerateModel(std::shared_ptr<Chunk>& chunk);
    bool ProcessNextModel();
    void GenerateView(Chunk& chunk, const Chunk* neighbors[kNeighborCount]);

    void BindTexture();

    const BlockRegistrar& GetBlockRegistrar() const {
      return *js_BlockRegistrar.Get();
    }

    BlockRegistrar& GetBlockRegistrar() {
      return *js_BlockRegistrar.Get();
    }

  private:

    std::queue<detail::loaded_chunks_data> loaded_chunks_;
  };

} // namespace chunklands::game

#endif