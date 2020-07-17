#ifndef __CHUNKLANDS_GAME_GAMEOVERLAY_HXX__
#define __CHUNKLANDS_GAME_GAMEOVERLAY_HXX__

#include <chunklands/js.hxx>
#include <chunklands/gl/glfw.hxx>
#include "SpriteRegistrar.hxx"
#include <chunklands/gl/Vao3fv3fn2ft.hxx>

namespace chunklands::game {

  class GameOverlay : public JSObjectWrap<GameOverlay> {
    JS_IMPL_WRAP(GameOverlay, ONE_ARG({
      JS_SETTER(SpriteRegistrar),
      JS_CB(setItemListActiveItem),
      JS_CB(getItemListActiveItem),
    }))

    JS_IMPL_SETTER_WRAP(SpriteRegistrar, SpriteRegistrar)
    JS_DECL_CB(getItemListActiveItem)
    JS_DECL_CB_VOID(setItemListActiveItem)

  public:
    void Prepare();
    void Update(double diff);
    void Render(double diff);

  private:
    gl::Vao3fv3fn2ft vao_crosshair_;
    gl::Vao3fv3fn2ft vao_itemlist_;

    struct {
      int active_item = 0;
    } itemlist_;
  };

} // namespace chunklands::game

#endif