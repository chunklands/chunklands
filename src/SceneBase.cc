#include "SceneBase.h"

namespace chunklands {
  Napi::FunctionReference SceneBase::constructor;

  void SceneBase::Initialize(Napi::Env env) {
    constructor = Napi::Persistent(
      DefineClass(env, "SceneBase", {
        InstanceMethod("prepare", &SceneBase::Prepare),
        InstanceMethod("render", &SceneBase::Render)
      })
    );

    constructor.SuppressDestruct();
  }

  SceneBase::SceneBase(const Napi::CallbackInfo& info) : Napi::ObjectWrap<SceneBase>(info) {

  }

  void SceneBase::Prepare(const Napi::CallbackInfo& info) {
    
  }

  void SceneBase::Render(const Napi::CallbackInfo& info) {

  }
}