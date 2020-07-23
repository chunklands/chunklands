#ifndef __CHUNKLANDS_GL_TEXTURE_HXX__
#define __CHUNKLANDS_GL_TEXTURE_HXX__

#include "glfw.hxx"
#include <glm/vec2.hpp>

namespace chunklands::gl {

  /**
   * @deprecated Use Texture2
   */
  class Texture {
  public:
    ~Texture();

  public:
    void LoadTexture(const char *filename);
    void ActiveAndBind(GLenum texture) const;

    const glm::ivec2& GetSize() const {
      return size_;
    }
  
  private:
    GLuint texture_ = 0;
    glm::ivec2 size_;
  };

} // namespace chunklands::gl

#endif