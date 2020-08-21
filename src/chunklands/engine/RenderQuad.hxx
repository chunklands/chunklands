#ifndef __CHUNKLANDS_ENGINE_RENDER_QUAD_HXX__
#define __CHUNKLANDS_ENGINE_RENDER_QUAD_HXX__

#include "gl/Vao.hxx"

namespace chunklands::engine {

  class RenderQuad {
  public:
    RenderQuad();

    void Render() const {
      vao_.Render();
    }

  private:
    gl::Vao<GL_TRIANGLE_STRIP, gl::VaoElementRenderQuad> vao_;
  };

} // namespace chunklands::engine

#endif