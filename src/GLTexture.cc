#include "GLTexture.h"

#include "stb.h"
#include <stdexcept>

namespace chunklands {

  GLTexture::~GLTexture() {
    if (texture_ != 0) {
      glDeleteTextures(1, &texture_);
      texture_ = 0;
    }
  }

  void GLTexture::LoadTexture(const char *filename) {
    int width, height, comp;
    auto&& data = stbi_load(filename, &width, &height, &comp, 0);
    assert(data != nullptr);
    if (!data) {
      throw std::runtime_error(std::string("could not load ") + filename);
    }

    GLenum format;
    switch (comp) {
    case 3: {
      format = GL_RGB;
      break;
    }
    case 4: {
      format = GL_RGBA;
      break;
    }
    default: {
      throw std::runtime_error(std::string("only support 3 or 4 channels"));
    }
    }

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);
    data = nullptr;
  }

  
  void GLTexture::ActiveAndBind(GLenum texture) {
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, texture_);
  }
}
