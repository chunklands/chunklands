#include "GameLoopBase.h"

#include <iostream>
#include "gl.h"
#include "prof.h"

namespace chunklands {
  JS_DEF_WRAP(GameLoopBase)

  void GameLoopBase::JSCall_start(JSCbi info) {
    auto&& env = info.Env();
    JS_ASSERT(!running_);

    js_Scene->Prepare();

    running_ = true;
    last_update_ = glfwGetTime();
    last_render_ = last_update_;
  }

  void GameLoopBase::JSCall_stop(JSCbi info) {
    auto&& env = info.Env();
    JS_ASSERT(running_);
    running_ = false;
  }

  constexpr double update_threshold = 1.0 / 30.0; // Hz

  void GameLoopBase::JSCall_loop(JSCbi info) {
    auto&& env = info.Env();
    JS_ASSERT(running_);

    const double time = glfwGetTime();

    { // update
      double update_diff = time - last_update_;
      while (update_diff >= update_threshold) {
        update_diff -= update_threshold;
        js_Scene->Update(update_threshold);
      }

      last_update_ = time - update_diff;
    }

    { // render
      const double render_diff = time - last_render_;
      last_render_ = time;
      js_Scene->Render(render_diff);
    }
  }
}