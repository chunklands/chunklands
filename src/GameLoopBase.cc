#include "GameLoopBase.h"
#include "gl.h"

namespace chunklands {
  Napi::FunctionReference GameLoopBase::constructor;

  void GameLoopBase::Initialize(Napi::Env env) {
    constructor = Napi::Persistent(DefineClass(env, "GameLoopBase", {
      InstanceMethod("loop", &GameLoopBase::Loop),
      InstanceMethod("start", &GameLoopBase::Start),
      InstanceMethod("stop", &GameLoopBase::Stop),
      InstanceMethod("setScene", &GameLoopBase::SetScene)
    }));

    constructor.SuppressDestruct();
  }

  GameLoopBase::GameLoopBase(const Napi::CallbackInfo& info) : Napi::ObjectWrap<GameLoopBase>(info) {

  }

  void GameLoopBase::Start(const Napi::CallbackInfo& info) {
    assert(!running_);

    scene_->Prepare();

    running_ = true;
    last_update_ = glfwGetTime();
  }

  void GameLoopBase::Stop(const Napi::CallbackInfo& info) {
    assert(running_);
    running_ = false;
  }

  void GameLoopBase::Loop(const Napi::CallbackInfo& info) {
    assert(running_);

    double time = glfwGetTime();
    double diff = time - last_update_;
    last_update_ = time;

    scene_->Render(diff);
  }

  void GameLoopBase::SetScene(const Napi::CallbackInfo& info) {
    scene_ = info[0].ToObject();
  }
}