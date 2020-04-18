#ifndef __CHUNKLANDS_RENDERQUAD_H__
#define __CHUNKLANDS_RENDERQUAD_H__

#include "gl.h"

namespace chunklands {
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
}

#endif
