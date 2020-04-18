#ifndef __CHUNKLANDS_GLSKYBOXTEXTURE_H__
#define __CHUNKLANDS_GLSKYBOXTEXTURE_H__

#include "gl.h"
#include <string>

namespace chunklands {
  class GLSkyboxTexture {
  public:
    ~GLSkyboxTexture();

  public:
    void LoadTexture(const std::string& prefix);
    void ActiveAndBind(GLenum texture);
  
  private:
    GLuint texture_ = 0;
  };
}

#endif
