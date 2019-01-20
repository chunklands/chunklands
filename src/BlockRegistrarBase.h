#ifndef __CHUNKLANDS_BLOCKREGISTRARBASE_H__
#define __CHUNKLANDS_BLOCKREGISTRARBASE_H__

#include <memory>
#include <napi.h>
#include <unordered_map>
#include "BlockDefinition.h"
#include "napi/object_wrap_util.h"

namespace chunklands {
  class BlockRegistrarBase : public Napi::ObjectWrap<BlockRegistrarBase> {
    DECLARE_OBJECT_WRAP(BlockRegistrarBase)
    DECLARE_OBJECT_WRAP_CB(void AddBlock)

  public:
    BlockDefinition* Find(const std::string& block_id);

  private:
    std::unordered_map<std::string, std::unique_ptr<BlockDefinition>> block_definitions_;
  };
}

#endif
