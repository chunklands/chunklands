#ifndef __CHUNKLANDS_CHUNKGENERATORBASE_H__
#define __CHUNKLANDS_CHUNKGENERATORBASE_H__

#include <napi.h>
#include "Chunk.h"
#include "napi/object_wrap_util.h"

namespace chunklands {
  class ChunkGeneratorBase : public Napi::ObjectWrap<ChunkGeneratorBase> {
    DECLARE_OBJECT_WRAP(ChunkGeneratorBase)

  public:
    void Generate(Chunk& chunk);
  };
}

#endif
