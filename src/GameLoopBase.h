#ifndef __CHUNKLANDS_GAMELOOPBASE_H__
#define __CHUNKLANDS_GAMELOOPBASE_H__

#include "js.h"
#include "SceneBase.h"

namespace chunklands {
  class GameLoopBase : public JSWrap<GameLoopBase> {
    JS_DECL_WRAP(GameLoopBase)
    JS_DECL_CB_VOID(start)
    JS_DECL_CB_VOID(stop)
    JS_DECL_CB_VOID(loop)
    JS_DECL_SETTER_REF(SceneBase, Scene)

  private:
    bool running_ = false;
    double last_update_ = .0;
    double last_render_ = .0;
  };
}

#endif