#ifndef __CHUNKLANDS_ENGINE_FONT_FONT_HXX__
#define __CHUNKLANDS_ENGINE_FONT_FONT_HXX__

#include <chunklands/engine/gl/Texture.hxx>
#include <chunklands/engine/types.hxx>

namespace chunklands::engine::font {

struct Font {
    Font(std::string name, int size, int width, int height,
        std::map<uint32_t, CEFontInitCharacter> characters,
        std::unique_ptr<gl::Texture> texture)
        : name(std::move(name))
        , size(size)
        , width(width)
        , height(height)
        , characters(std::move(characters))
        , texture(std::move(texture))
    {
    }

    std::string name;
    int size;
    int width;
    int height;
    std::map<uint32_t, CEFontInitCharacter> characters;
    std::unique_ptr<gl::Texture> texture;
};

} // namespace chunklands::engine::font

#endif