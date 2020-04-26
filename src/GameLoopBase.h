#ifndef __CHUNKLANDS_GAMELOOPBASE_H__
#define __CHUNKLANDS_GAMELOOPBASE_H__

#include "js.h"
#include "SceneBase.h"

namespace chunklands {
  class GameLoopBase : public JSObjectWrap<GameLoopBase> {
    JS_IMPL_WRAP(GameLoopBase, ONE_ARG({
      JS_CB(start),
      JS_CB(stop),
      JS_CB(loop),
      JS_SETTER(Scene)
    }))

    JS_DECL_CB_VOID(start)
    JS_DECL_CB_VOID(stop)
    JS_DECL_CB_VOID(loop)
    JS_IMPL_SETTER_WRAP(SceneBase, Scene)

  private:
    bool running_ = false;
    double last_update_ = .0;
    double last_render_ = .0;
  };
}

#endif