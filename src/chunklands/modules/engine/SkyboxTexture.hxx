#ifndef __CHUNKLANDS_MODULES_ENGINE_SKYBOXTEXTURE_HXX__
#define __CHUNKLANDS_MODULES_ENGINE_SKYBOXTEXTURE_HXX__

#include <string>
#include <chunklands/modules/gl/glfw.hxx>

namespace chunklands::modules::engine {

  class SkyboxTexture {
  public:
    ~SkyboxTexture() {
      glDeleteTextures(1, &texture_);
    }

  public:
    void LoadTexture(const std::string& prefix);
    void ActiveAndBind(GLenum texture) {
      glActiveTexture(texture);
      glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
    }
  
  private:
    GLuint texture_ = 0;
  };

} // namespace chunklands::modules::engine

#endif