#ifndef __CHUNKLANDS_ENGINE_TEXT_TEXT_HXX__
#define __CHUNKLANDS_ENGINE_TEXT_TEXT_HXX__

#include <chunklands/engine/gl/Vao.hxx>
#include <chunklands/engine/types.hxx>

namespace chunklands::engine::text {

struct Text {

    Text(CEFontHandle* font)
        : font(font)
    {
        assert(font != nullptr);
    }

    CEFontHandle* font;
    std::string text;
    glm::ivec2 pos;
    gl::Vao<GL_TRIANGLES, CEVaoElementText> vao;
};

} // namespace chunklands::engine::text

#endif