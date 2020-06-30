#ifndef __CHUNKLANDS_MODULES_ENGINE_FONTLOADER_HXX__
#define __CHUNKLANDS_MODULES_ENGINE_FONTLOADER_HXX__

#include <chunklands/js.hxx>
#include <glm/vec2.hpp>
#include <chunklands/modules/gl/gl_module.hxx>
#include <unordered_map>
#include <optional>

namespace chunklands::modules::engine {

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
  
    const std::optional<font_loader_char> Get(const std::string& ch) const {
      auto&& it = meta_.find(ch);
      if (it == meta_.cend()) {
        return {};
      }

      return {it->second};
    }

    const gl::Texture& GetTexture() const {
      return texture_;
    }

  private:
    std::unordered_map<std::string, font_loader_char> meta_;
    gl::Texture texture_;
  };

} // namespace chunklands::modules::engine

#endif