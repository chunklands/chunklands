#ifndef __CHUNKLANDS_ENGINE_SPRITE_SPRITE_HXX__
#define __CHUNKLANDS_ENGINE_SPRITE_SPRITE_HXX__

#include <chunklands/engine/gl/Vao.hxx>
#include <chunklands/engine/types.hxx>

namespace chunklands::engine::sprite {

struct Sprite {
    Sprite(std::string id, std::vector<CEVaoElementSprite> vao_elements)
        : id(std::move(id))
        , vao_elements(std::move(vao_elements))
    {
    }

    std::string id;
    std::vector<CEVaoElementSprite> vao_elements;
    gl::Vao<GL_TRIANGLES, CEVaoElementSprite> vao;
};

} // namespace chunklands::engine::sprite

#endif