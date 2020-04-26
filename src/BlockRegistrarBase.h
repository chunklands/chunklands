#ifndef __CHUNKLANDS_BLOCKREGISTRARBASE_H__
#define __CHUNKLANDS_BLOCKREGISTRARBASE_H__

#include <memory>
#include <vector>
#include "BlockDefinition.h"
#include "js.h"
#include "GLTexture.h"

namespace chunklands {
  class BlockRegistrarBase : public JSObjectWrap<BlockRegistrarBase> {
    JS_IMPL_WRAP(BlockRegistrarBase, ONE_ARG({
      JS_CB(addBlock),
      JS_CB(loadTexture),
      JS_CB(getBlockIds)
    }))

    JS_DECL_CB_VOID(addBlock)
    JS_DECL_CB_VOID(loadTexture)
    JS_DECL_CB(getBlockIds)

  public:
    BlockDefinition* GetByIndex(int index);
    void BindTexture();

  private:
    std::vector<std::unique_ptr<BlockDefinition>> block_definitions_;

    GLTexture texture_;
  };
}

#endif
