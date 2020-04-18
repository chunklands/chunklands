#ifndef __CHUNKLANDS_TEXTURELOADER_H__
#define __CHUNKLANDS_TEXTURELOADER_H__

#include "stb.h"
#include "gl.h"

#include <iostream>

namespace chunklands {
  class TextureLoader {
  public:
    TextureLoader(const std::string& filename);
    ~TextureLoader();

  public:
    stbi_uc* data = nullptr;
    GLenum format = GL_NONE;

    int width = 0;
    int height = 0;
  };

  std::ostream& operator<<(std::ostream& os, const TextureLoader& obj);
}

#endif
