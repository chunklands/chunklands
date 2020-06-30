
#include "SkyboxTexture.hxx"

#include <chunklands/modules/gl/gl_module.hxx>

namespace chunklands::engine {

  void SkyboxTexture::LoadTexture(const std::string& prefix) {
    CHECK_GL();

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);

    {
      CHECK_GL_HERE();
      modules::gl::TextureLoader loader(prefix + "right.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL_HERE();
      modules::gl::TextureLoader loader(prefix + "left.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL_HERE();
      modules::gl::TextureLoader loader(prefix + "top.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL_HERE();
      modules::gl::TextureLoader loader(prefix + "bottom.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL_HERE();
      modules::gl::TextureLoader loader(prefix + "front.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL_HERE();
      modules::gl::TextureLoader loader(prefix + "back.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }

} // namespace chunklands::engine