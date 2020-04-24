#ifndef __CHUNKLANDS_BLOCKREGISTRARBASE_H__
#define __CHUNKLANDS_BLOCKREGISTRARBASE_H__

#include <memory>
#include <vector>
#include "BlockDefinition.h"
#include "js.h"
#include "GLTexture.h"

namespace chunklands {
  class BlockRegistrarBase : public JSWrap<BlockRegistrarBase> {
    JS_DECL_WRAP(BlockRegistrarBase)
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
