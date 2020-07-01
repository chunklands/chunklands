#ifndef __CHUNKLANDS_GAME_GAMEOVERLAY_HXX__
#define __CHUNKLANDS_GAME_GAMEOVERLAY_HXX__

#include <chunklands/js.hxx>
#include <chunklands/gl/glfw.hxx>
#include "SpriteRegistrar.hxx"

namespace chunklands::game {

  class GameOverlay : public JSObjectWrap<GameOverlay> {
    JS_IMPL_WRAP(GameOverlay, ONE_ARG({
      JS_SETTER(SpriteRegistrar),
    }))

    JS_IMPL_SETTER_WRAP(SpriteRegistrar, SpriteRegistrar)

  public:
    void Prepare();
    void Update(double diff);
    void Render(double diff);

  private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;

    GLsizei vb_index_count_ = 0;
  };

} // namespace chunklands::game

#endif