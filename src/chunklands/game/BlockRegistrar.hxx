#ifndef __CHUNKLANDS_GAME_BLOCKREGISTRAR_HXX__
#define __CHUNKLANDS_GAME_BLOCKREGISTRAR_HXX__

#include <chunklands/js.hxx>
#include "BlockDefinition.hxx"
#include <chunklands/gl/Texture.hxx>

namespace chunklands::game {

  class BlockRegistrar : public JSObjectWrap<BlockRegistrar> {
    JS_IMPL_WRAP(BlockRegistrar, ONE_ARG({
      JS_CB(addBlock),
      JS_CB(loadTexture)
    }))

    JS_DECL_CB(addBlock)
    JS_DECL_CB_VOID(loadTexture)

  public:
    BlockDefinition* GetByIndex(int index);
    const BlockDefinition* GetByIndex(int index) const;
    void BindTexture();

  private:
    std::vector<std::unique_ptr<BlockDefinition>> block_definitions_;

    gl::Texture texture_;
  };

} // namespace chunklands::game

#endif