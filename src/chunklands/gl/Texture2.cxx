
#include "Texture2.hxx"
#include "TextureLoader.hxx"

namespace chunklands::gl {

  JS_DEF_WRAP(Texture2)

  Texture2::~Texture2() {
    if (texture_ != 0) {
      glDeleteTextures(1, &texture_);
      texture_ = 0;
    }
  }

  void Texture2::JSCall_load(JSCbi info) {
    CHECK_GL();
    std::string filename = info[0].ToString();
    TextureLoader loader(filename);

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    size_.x = loader.width;
    size_.y = loader.height;
  }

  void Texture2::ActiveAndBind(GLenum active_texture) const {
    glActiveTexture(active_texture);
    glBindTexture(GL_TEXTURE_2D, texture_);
  }

} // namespace chunklands::gl