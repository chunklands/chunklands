
#include "GameLoop.hxx"
#include <chunklands/gl/glfw.hxx>
#include <chunklands/modules/misc/misc_module.hxx>

namespace chunklands::engine {

  JS_DEF_WRAP(GameLoop)

  void GameLoop::JSCall_start(JSCbi info) {
    JS_ASSERT(info.Env(), !running_);

    js_Scene->Prepare();

    running_ = true;
    last_update_ = glfwGetTime();
    last_render_ = last_update_;
  }

  void GameLoop::JSCall_stop(JSCbi info) {
    JS_ASSERT(info.Env(), running_);
    running_ = false;
  }

  // constexpr double update_threshold = 1.0 / 30.0; // Hz

  void GameLoop::JSCall_loop(JSCbi info) {
    DURATION_METRIC("gameloop_loop");

    JS_ASSERT(info.Env(), running_);

    const double time = glfwGetTime();
    // { // update
    //   double update_diff = time - last_update_;
    //   while (update_diff >= update_threshold) {
    //     update_diff -= update_threshold;
    //     js_Scene->Update(update_threshold);
    //   }

    //   last_update_ = time - update_diff;
    // }

    { // update
      double update_diff = time - last_update_;
      js_Scene->Update(update_diff);

      last_update_ = time;
    }

    { // render
      const double render_diff = time - last_render_;
      last_render_ = time;
      js_Scene->Render(render_diff);
    }

    glfwPollEvents();
  }

} // namespace chunklands::engine