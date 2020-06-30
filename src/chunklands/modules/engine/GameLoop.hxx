#ifndef __CHUNKLANDS_MODULES_ENGINE_GAMELOOP_HXX__
#define __CHUNKLANDS_MODULES_ENGINE_GAMELOOP_HXX__

#include <chunklands/js.hxx>
#include "IScene.hxx"

namespace chunklands::modules::engine {

  class GameLoop : public JSObjectWrap<GameLoop> {
    JS_IMPL_WRAP(GameLoop, ONE_ARG({
      JS_CB(start),
      JS_CB(stop),
      JS_CB(loop),
      JS_SETTER(Scene)
    }))

    JS_DECL_CB_VOID(start)
    JS_DECL_CB_VOID(stop)
    JS_DECL_CB_VOID(loop)
    JS_IMPL_ABSTRACT_WRAP_SETTER(IScene, Scene)

  private:
    bool running_ = false;
    double last_update_ = .0;
    double last_render_ = .0;
  };

} // namespace chunklands::modules::engine

#endif