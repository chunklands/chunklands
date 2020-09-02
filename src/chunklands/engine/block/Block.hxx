#ifndef __CHUNKLANDS_ENGINE_BLOCK_BLOCK_HXX__
#define __CHUNKLANDS_ENGINE_BLOCK_BLOCK_HXX__

#include <chunklands/engine/algorithm/texture_bake.hxx>
#include <chunklands/engine/gl/Vao.hxx>
#include <chunklands/engine/types.hxx>
#include <string>
#include <vector>

namespace chunklands::engine::block {

struct Block {
    Block(std::string id, bool opaque, std::vector<CEBlockFace> faces)
        : id(std::move(id))
        , faces(std::move(faces))
        , opaque(opaque)
    {
    }

    std::string id;
    std::vector<CEBlockFace> faces;
    bool opaque;
};

} // namespace chunklands::engine::block

#endif