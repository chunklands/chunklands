#ifndef __CHUNKLANDS_GLTEXTURE_H__
#define __CHUNKLANDS_GLTEXTURE_H__

#include <string>

#include "gl.h"

namespace chunklands {
  class GLTexture {
  public:
    ~GLTexture();

  public:
    void LoadTexture(const char *filename);
    void ActiveAndBind(GLenum texture);
  
  private:
    GLuint texture_ = 0;
  };
}

#endif
