#ifndef __CHUNKLANDS_ENGINE_FONTLOADER_HXX__
#define __CHUNKLANDS_ENGINE_FONTLOADER_HXX__

#include <chunklands/js.hxx>
#include <glm/vec2.hpp>
#include <chunklands/gl/Texture.hxx>
#include <unordered_map>
#include <optional>

namespace chunklands::engine {

  struct font_loader_char {
    glm::ivec2  pos,
                size,
                origin;
    int         advance;
  };

  class FontLoader : public JSObjectWrap<FontLoader> {
    JS_IMPL_WRAP(FontLoader, ONE_ARG({
      JS_CB(load),
    }))

    JS_DECL_CB_VOID(load)

  public:
  
    const std::optional<font_loader_char> Get(const std::string& ch) const;

    const gl::Texture& GetTexture() const {
      return texture_;
    }

  private:
    std::unordered_map<std::string, font_loader_char> meta_;
    gl::Texture texture_;
  };

} // namespace chunklands::engine

#endif