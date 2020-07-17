
#include "GameOverlay.hxx"

namespace chunklands::game {

  JS_DEF_WRAP(GameOverlay)

  JSValue GameOverlay::JSCall_getItemListActiveItem(JSCbi info) {
    return JSNumber::New(info.Env(), itemlist_.active_item);
  }

  void GameOverlay::JSCall_setItemListActiveItem(JSCbi info) {
    itemlist_.active_item = info[0].ToNumber();
  }

  void GameOverlay::Prepare() {

    // crosshair
    {
      const sprite_definition* sprite = js_SpriteRegistrar->GetSprite("sprite.crosshair");
      assert(sprite);

      const std::vector<GLfloat>& all = sprite->faces.at("all");
      vao_crosshair_.Init(GL_TRIANGLES, all.data(), all.size());
    }

    // itemlist
    {
      const sprite_definition* sprite = js_SpriteRegistrar->GetSprite("sprite.item-list");
      assert(sprite);

      const std::vector<GLfloat>& all = sprite->faces.at("all");
      vao_itemlist_.Init(GL_TRIANGLES, all.data(), all.size());
    }
  }

  void GameOverlay::Update(double) {

  }

  void GameOverlay::Render(double) {
    vao_crosshair_.Render();
    vao_itemlist_.Render();
  }


} // namespace chunklands::game