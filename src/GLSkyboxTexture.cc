#include "GLSkyboxTexture.h"

#include "TextureLoader.h"

#include <iostream>

namespace chunklands {

  GLSkyboxTexture::~GLSkyboxTexture() {
    if (texture_ != 0) {
      glDeleteTextures(1, &texture_);
      texture_ = 0;
    }
  }

  void GLSkyboxTexture::LoadTexture(const std::string& prefix) {

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);

    {
      CHECK_GL();
      TextureLoader loader(prefix + "right.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL();
      TextureLoader loader(prefix + "left.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL();
      TextureLoader loader(prefix + "top.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL();
      TextureLoader loader(prefix + "bottom.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL();
      TextureLoader loader(prefix + "front.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL();
      TextureLoader loader(prefix + "back.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
  
  void GLSkyboxTexture::ActiveAndBind(GLenum texture) {
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
  }
}
