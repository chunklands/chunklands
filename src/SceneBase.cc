#include "SceneBase.h"

namespace chunklands {
  DEFINE_OBJECT_WRAP_DEFAULT_CTOR(SceneBase, ONE_ARG({
    InstanceMethod("setWindow", &SceneBase::SetWindow),
    InstanceMethod("setWorld", &SceneBase::SetWorld)
  }))

  void SceneBase::SetWindow(const Napi::CallbackInfo& info) {
    window_ = info[0].ToObject();
    UpdateViewport();

    window_on_resize_conn_ = window_->on_resize.connect([this](int width, int height) {
      UpdateViewport(width, height);
    });

    window_on_cursor_move_conn_ = window_->on_cursor_move.connect([this](double xpos, double ypos) {
      glm::ivec2 current_cursor_pos(xpos, ypos);
      glm::ivec2 cursor_diff = last_cursor_pos_ - current_cursor_pos;
      last_cursor_pos_ = current_cursor_pos;

      const float yaw_rad   = .005f * cursor_diff.x;
      const float pitch_rad = .005f * cursor_diff.y;

      world_->AddLook(yaw_rad, pitch_rad);
    });
  }

  void SceneBase::SetWorld(const Napi::CallbackInfo& info) {
    world_ = info[0].ToObject();
    UpdateViewport();
  }

  void SceneBase::Prepare() {
    world_->Prepare();
  }

  void SceneBase::Update(double diff) {
    constexpr float move_factor = 20.f;

    auto&& look = world_->GetLook();

    if (window_->GetKey(GLFW_KEY_W) == GLFW_PRESS) {
      glm::vec3 move(-sinf(look.x) * cosf(look.y),
                     sinf(look.y),
                     -cosf(look.x) * cosf(look.y));
      world_->AddPos(move_factor * (float)diff * move);
    }

    if (window_->GetKey(GLFW_KEY_S) == GLFW_PRESS) {
      glm::vec3 move(-sinf(look.x) * cosf(look.y),
                     sinf(look.y),
                     -cosf(look.x) * cosf(look.y));
      world_->AddPos(-move_factor * (float)diff * move);
    }

    if (window_->GetKey(GLFW_KEY_A) == GLFW_PRESS) {
      glm::vec3 move(-cosf(look.x) * cosf(look.y),
                     0.f,
                     sinf(look.x) * cosf(look.y));
      world_->AddPos(move_factor * (float)diff * move);
    }

    if (window_->GetKey(GLFW_KEY_D) == GLFW_PRESS) {
      glm::vec3 move(-cosf(look.x) * cosf(look.y),
                     0.f,
                     sinf(look.x) * cosf(look.y));
      world_->AddPos(-move_factor * (float)diff * move);
    }

    if (window_->GetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      window_->StartMouseGrab();
      last_cursor_pos_ = window_->GetCursorPos();
    }

    if (window_->GetKey(GLFW_KEY_ESCAPE)) {
      window_->StopMouseGrab();
    }

    world_->Update(diff);

    glfwPollEvents();
  }

  void SceneBase::Render(double diff) {
    world_->Render(diff);    
    window_->SwapBuffers();
  }

  void SceneBase::UpdateViewport() {
    if (window_.IsEmpty()) {
      return;
    }

    glm::ivec2 size = window_->GetSize();
    UpdateViewport(size.x, size.y);
  }

  void SceneBase::UpdateViewport(int width, int height) {
    glViewport(0, 0, width, height);
    if (!world_.IsEmpty()) {
      world_->UpdateViewportRatio(width, height);
    }
  }
}