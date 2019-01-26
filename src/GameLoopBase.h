#ifndef __CHUNKLANDS_GAMELOOPBASE_H__
#define __CHUNKLANDS_GAMELOOPBASE_H__

#include <napi.h>
#include "napi/object_wrap_util.h"
#include "napi/PersistentObjectWrap.h"
#include "SceneBase.h"

namespace chunklands {
  class GameLoopBase : public Napi::ObjectWrap<GameLoopBase> {
    DECLARE_OBJECT_WRAP(GameLoopBase)
    DECLARE_OBJECT_WRAP_CB(void Start)
    DECLARE_OBJECT_WRAP_CB(void Stop)
    DECLARE_OBJECT_WRAP_CB(void Loop)
    DECLARE_OBJECT_WRAP_CB(void SetScene)

  private:
    bool running_ = false;
    double last_update_ = .0;
    double last_render_ = .0;

    NapiExt::PersistentObjectWrap<SceneBase> scene_;
  };
}

#endif