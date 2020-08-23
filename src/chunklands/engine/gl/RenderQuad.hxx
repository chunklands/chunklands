#ifndef __CHUNKLANDS_ENGINE_GL_RENDERQUAD_HXX__
#define __CHUNKLANDS_ENGINE_GL_RENDERQUAD_HXX__

#include <chunklands/engine/gl/Vao.hxx>

namespace chunklands::engine::gl {

class RenderQuad {
public:
    RenderQuad();

    void Render() const
    {
        vao_.Render();
    }

private:
    Vao<GL_TRIANGLE_STRIP, VaoElementRenderQuad> vao_;
};

} // namespace chunklands::engine::gl

#endif