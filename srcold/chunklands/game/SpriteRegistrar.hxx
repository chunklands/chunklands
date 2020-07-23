#ifndef __CHUNKLANDS_GAME_SPRITEREGISTRAR_HXX__
#define __CHUNKLANDS_GAME_SPRITEREGISTRAR_HXX__

#include <chunklands/js.hxx>
#include <string>
#include <unordered_map>
#include <vector>
#include <chunklands/gl/glfw.hxx>

namespace chunklands::game {

  struct sprite_definition {
    std::string id;
    std::unordered_map<std::string, std::vector<GLfloat>> faces;
  };

  class SpriteRegistrar : public JSObjectWrap<SpriteRegistrar> {
    JS_IMPL_WRAP(SpriteRegistrar, ONE_ARG({
      JS_CB(addSprite),
    }))

    JS_DECL_CB_VOID(addSprite)

  public:
    const sprite_definition* GetSprite(const std::string& id) const;

  private:
    std::unordered_map<std::string, std::unique_ptr<sprite_definition>> sprites_;
  };

} // namespace chunklands::game

#endif