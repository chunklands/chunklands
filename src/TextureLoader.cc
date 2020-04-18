#include "TextureLoader.h"

#include <string>
#include <stdexcept>

namespace chunklands {

  TextureLoader::TextureLoader(const std::string& filename) {
    int comp;
    data = stbi_load(filename.c_str(), &width, &height, &comp, 0);
    
    if (!data) {
      throw std::runtime_error(std::string("could not load ") + filename);
    }

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
  }

  TextureLoader::~TextureLoader() {
    if (data != nullptr) {
      stbi_image_free(data);
      data = nullptr;
    }
  }

  std::ostream& operator<<(std::ostream& os, const TextureLoader& obj) {
    return os
      << "TextureLoader {\n"
      << "  data   = " << (void *)obj.data << "\n"
      << "  format = " << obj.format << "\n"
      << "  width  = " << obj.width << "\n"
      << "  height = " << obj.height << "\n"
      << "}"
      << std::endl;
  }
}
