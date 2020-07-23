#ifndef __CHUNKLANDS_GL_TEXTURELOADER_HXX__
#define __CHUNKLANDS_GL_TEXTURELOADER_HXX__

#include <string>
#include <ostream>
#include "glfw.hxx"
#include <stb_image.h>

namespace chunklands::gl {

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

} // namespace chunklands::gl

#endif