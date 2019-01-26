#include "GameLoopBase.h"

#include <iostream>
#include "gl.h"

namespace chunklands {
  DEFINE_OBJECT_WRAP_DEFAULT_CTOR(GameLoopBase, ONE_ARG({
    InstanceMethod("loop", &GameLoopBase::Loop),
    InstanceMethod("start", &GameLoopBase::Start),
    InstanceMethod("stop", &GameLoopBase::Stop),
    InstanceMethod("setScene", &GameLoopBase::SetScene)
  }))

  void GameLoopBase::Start(const Napi::CallbackInfo& info) {
    assert(!running_);

    scene_->Prepare();

    running_ = true;
    last_update_ = glfwGetTime();
    last_render_ = last_update_;
  }

  void GameLoopBase::Stop(const Napi::CallbackInfo& info) {
    assert(running_);
    running_ = false;
  }

  constexpr double update_threshold = 1.0 / 30.0; // Hz

  void GameLoopBase::Loop(const Napi::CallbackInfo& info) {
    assert(running_);

    const double time = glfwGetTime();

    std::cout << "loop: ";

    { // update
      double update_diff = time - last_update_;
      while (update_diff >= update_threshold) {
        update_diff -= update_threshold;

        std::cout << "[update " << (int)(update_threshold*1000.0) << "ms] ";
        scene_->Update(update_threshold);
      }

      last_update_ = time - update_diff;
    }

    { // render
      const double render_diff = time - last_render_;
      last_render_ = time;

      std::cout << "[render " << (int)(render_diff*1000.0) << "ms] " << std::endl;
      scene_->Render(render_diff);
    }
  }

  void GameLoopBase::SetScene(const Napi::CallbackInfo& info) {
    scene_ = info[0].ToObject();
  }
}