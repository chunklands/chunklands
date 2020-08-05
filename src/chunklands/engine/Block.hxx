#ifndef __CHUNKLANDS_ENGINE_BLOCK_HXX__
#define __CHUNKLANDS_ENGINE_BLOCK_HXX__

#include <string>
#include <vector>
#include "gl/Vao.hxx"
#include "api-types.hxx"

namespace chunklands::engine {

  struct Block {
    Block(std::string id, bool opaque, std::vector<CEVaoElementChunkBlock> data)
      : id(std::move(id))
      , data(std::move(data))
      , opaque(opaque)
      {}

    std::string id;
    std::vector<CEVaoElementChunkBlock> data;
    bool opaque;
  };

} // namespace chunklands::engine

#endif