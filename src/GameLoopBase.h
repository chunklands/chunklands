#ifndef __CHUNKLANDS_GAMELOOPBASE_H__
#define __CHUNKLANDS_GAMELOOPBASE_H__

#include <napi.h>
#include "napi/UnwrappedObject.h"
#include "SceneBase.h"

namespace chunklands {
  class GameLoopBase : public Napi::ObjectWrap<GameLoopBase> {
  public:
    static void Initialize(Napi::Env env);
    static Napi::FunctionReference constructor;

  public:
    GameLoopBase(const Napi::CallbackInfo& info);

  public: // JS
    void Start(const Napi::CallbackInfo& info);
    void Stop(const Napi::CallbackInfo& info);
    void Loop(const Napi::CallbackInfo& info);

    void SetScene(const Napi::CallbackInfo& info);

  public: // Native

  private:
    bool running_ = false;
    double last_update_ = .0;
    double last_render_ = .0;

    NapiExt::PersistentObjectWrap<SceneBase> scene_;
  };
}

#endif