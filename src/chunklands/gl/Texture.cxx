
#include "Texture.hxx"
#include "TextureLoader.hxx"

namespace chunklands::gl {

  Texture::~Texture() {
    if (texture_ != 0) {
      glDeleteTextures(1, &texture_);
      texture_ = 0;
    }
  }

  void Texture::LoadTexture(const char* filename) {
    CHECK_GL();
    TextureLoader loader(filename);

    // TODO(daaitch): load POT dimension texture
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    size_.x = loader.width;
    size_.y = loader.height;
  }
  
  void Texture::ActiveAndBind(GLenum texture) const {
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, texture_);
  }

} // namespace chunklands::gl