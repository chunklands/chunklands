#ifndef __CHUNKLANDS_GL_RENDERQUAD_HXX__
#define __CHUNKLANDS_GL_RENDERQUAD_HXX__

#include "glfw.hxx"

namespace chunklands::gl {

  class RenderQuad {
  public:
    RenderQuad();
    ~RenderQuad();

  public:
    void Render();

  private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
  };

} // namespace chunklands::gl

#endif