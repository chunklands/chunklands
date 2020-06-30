#ifndef __CHUNKLANDS_GL_RENDERQUAD_HXX__
#define __CHUNKLANDS_GL_RENDERQUAD_HXX__

#include "glfw.hxx"

namespace chunklands::gl {

  class RenderQuad {
  public:
    RenderQuad();
    ~RenderQuad();

  public:
    void Render() {
      glBindVertexArray(vao_);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      glBindVertexArray(0);
    }

  private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
  };

} // namespace chunklands::gl

#endif