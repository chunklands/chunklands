#ifndef __CHUNKLANDS_RENDERQUAD_H__
#define __CHUNKLANDS_RENDERQUAD_H__

#include "gl.h"

namespace chunklands {
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
}

#endif
