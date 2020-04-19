#include "SceneBase.h"

#include <random>
#include <glm/geometric.hpp>

namespace chunklands {
  DEFINE_OBJECT_WRAP_DEFAULT_CTOR(SceneBase, ONE_ARG({
    InstanceMethod("setWindow", &SceneBase::SetWindow),
    InstanceMethod("setWorld", &SceneBase::SetWorld)
  }))

  SceneBase::~SceneBase() {
    DeleteGLBuffers();
  }

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
    CHECK_GL();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    { // g-buffer pass
      CHECK_GL_HERE();
      glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_.framebuffer);
      world_->RenderGBufferPass(diff);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      CHECK_GL_HERE();
    }

    {
      CHECK_GL_HERE();
      glBindFramebuffer(GL_FRAMEBUFFER, ssao_.framebuffer);
      world_->RenderSSAOPass(diff, g_buffer_.position_texture, g_buffer_.normal_texture, ssao_.noise_texture);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      CHECK_GL_HERE();
    }

    { // deferred lighting pass
      world_->RenderDeferredLightingPass(diff, g_buffer_.position_texture, g_buffer_.normal_texture, g_buffer_.color_texture, ssao_.color_texture);
    }

    { // skybox
      world_->RenderSkybox(diff);
    }

    // glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer_.framebuffer);
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // glBlitFramebuffer(0, 0, buffer_size_.x, buffer_size_.y, 0, 0, buffer_size_.x, buffer_size_.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

    DeleteGLBuffers();
    InitializeGLBuffers(width, height);
  }
  
  void SceneBase::InitializeGLBuffers(int width, int height) {
    
    buffer_size_.x = width;
    buffer_size_.y = height;

    { // gBuffer
      glGenFramebuffers(1, &g_buffer_.framebuffer);
      glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_.framebuffer);

      glGenTextures(1, &g_buffer_.position_texture);
      glBindTexture(GL_TEXTURE_2D, g_buffer_.position_texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_buffer_.position_texture, 0);

      glGenTextures(1, &g_buffer_.normal_texture);
      glBindTexture(GL_TEXTURE_2D, g_buffer_.normal_texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_buffer_.normal_texture, 0);

      glGenTextures(1, &g_buffer_.color_texture);
      glBindTexture(GL_TEXTURE_2D, g_buffer_.color_texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_buffer_.color_texture, 0);

      const GLuint attachments[3] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
      };

      glDrawBuffers(3, attachments);

      glGenRenderbuffers(1, &g_buffer_.renderbuffer);
      glBindRenderbuffer(GL_RENDERBUFFER, g_buffer_.renderbuffer);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_buffer_.renderbuffer);

      assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    { // initialize SSAO
      glGenFramebuffers(1, &ssao_.framebuffer);
      glBindFramebuffer(GL_FRAMEBUFFER, ssao_.framebuffer);

      glGenTextures(1, &ssao_.color_texture);
      glBindTexture(GL_TEXTURE_2D, ssao_.color_texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_.color_texture, 0);
      
      assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      std::uniform_real_distribution<GLfloat> random_floats(0.f, 1.f);
      std::default_random_engine generator;

      std::array<glm::vec3, 16> ssao_noise;
      for (int i = 0; i < 16; i++) {
        glm::vec3 noise(
          random_floats(generator) * 2.f - 1.f,
          random_floats(generator) * 2.f - 1.f,
          0.f
        );
        ssao_noise[i] = std::move(noise);
      }

      glGenTextures(1, &ssao_.noise_texture);
      glBindTexture(GL_TEXTURE_2D, ssao_.noise_texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, ssao_noise.data());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
  }

  void SceneBase::DeleteGLBuffers() {
    if (g_buffer_.position_texture != 0) {
      glDeleteTextures(1, &g_buffer_.position_texture);
      g_buffer_.position_texture = 0;
    }

    if (g_buffer_.normal_texture != 0) {
      glDeleteTextures(1, &g_buffer_.normal_texture);
      g_buffer_.normal_texture = 0;
    }

    if (g_buffer_.color_texture != 0) {
      glDeleteTextures(1, &g_buffer_.color_texture);
      g_buffer_.color_texture = 0;
    }

    if (g_buffer_.framebuffer != 0) {
      glDeleteFramebuffers(1, &g_buffer_.framebuffer);
      g_buffer_.framebuffer = 0;
    }
  }
}