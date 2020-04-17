#ifndef __CHUNKLANDS_BLOCKREGISTRARBASE_H__
#define __CHUNKLANDS_BLOCKREGISTRARBASE_H__

#include <memory>
#include <napi.h>
#include <vector>
#include "BlockDefinition.h"
#include "gl.h"
#include "napi/object_wrap_util.h"

namespace chunklands {
  class BlockRegistrarBase : public Napi::ObjectWrap<BlockRegistrarBase> {
    DECLARE_OBJECT_WRAP(BlockRegistrarBase)
    DECLARE_OBJECT_WRAP_CB(void AddBlock)
    DECLARE_OBJECT_WRAP_CB(void LoadTexture)
    DECLARE_OBJECT_WRAP_CB(Napi::Value GetBlockIds)

  public:
    BlockDefinition* GetByIndex(int index);
    void BindTexture();

  private:
    std::vector<std::unique_ptr<BlockDefinition>> block_definitions_;

    GLuint texture_ = 0;
  };
}

#endif
