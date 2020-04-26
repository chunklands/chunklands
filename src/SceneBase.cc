#include "SceneBase.h"

#include <random>
#include <glm/geometric.hpp>

namespace chunklands {
  JS_DEF_WRAP(SceneBase)

  void SceneBase::JSCall_SetWindow(JSCbi info) {
    js_Window = info[0].ToObject();
    UpdateViewport();

    window_on_resize_conn_ = js_Window->on_resize.connect([this](int width, int height) {
      UpdateViewport(width, height);
    });

    window_on_cursor_move_conn_ = js_Window->on_cursor_move.connect([this](double xpos, double ypos) {
      glm::ivec2 current_cursor_pos(xpos, ypos);
      glm::ivec2 cursor_diff = last_cursor_pos_ - current_cursor_pos;
      last_cursor_pos_ = current_cursor_pos;

      const float yaw_rad   = .005f * cursor_diff.x;
      const float pitch_rad = .005f * cursor_diff.y;

      js_World->AddLook(yaw_rad, pitch_rad);
    });
  }

  void SceneBase::JSCall_SetWorld(JSCbi info) {
    js_World = info[0].ToObject();
    UpdateViewport();
  }

  void SceneBase::Update(double diff) {
    constexpr float move_factor = 20.f;

    auto&& look = js_World->GetLook();

    if (js_Window->GetKey(GLFW_KEY_W) == GLFW_PRESS) {
      glm::vec3 move(-sinf(look.x) * cosf(look.y),
                     sinf(look.y),
                     -cosf(look.x) * cosf(look.y));
      js_World->AddPos(move_factor * (float)diff * move);
    }

    if (js_Window->GetKey(GLFW_KEY_S) == GLFW_PRESS) {
      glm::vec3 move(-sinf(look.x) * cosf(look.y),
                     sinf(look.y),
                     -cosf(look.x) * cosf(look.y));
      js_World->AddPos(-move_factor * (float)diff * move);
    }

    if (js_Window->GetKey(GLFW_KEY_A) == GLFW_PRESS) {
      glm::vec3 move(-cosf(look.x) * cosf(look.y),
                     0.f,
                     sinf(look.x) * cosf(look.y));
      js_World->AddPos(move_factor * (float)diff * move);
    }

    if (js_Window->GetKey(GLFW_KEY_D) == GLFW_PRESS) {
      glm::vec3 move(-cosf(look.x) * cosf(look.y),
                     0.f,
                     sinf(look.x) * cosf(look.y));
      js_World->AddPos(-move_factor * (float)diff * move);
    }

    if (js_Window->GetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      js_Window->StartMouseGrab();
      last_cursor_pos_ = js_Window->GetCursorPos();
    }

    if (js_Window->GetKey(GLFW_KEY_ESCAPE)) {
      js_Window->StopMouseGrab();
    }

    js_World->Update(diff);

    glfwPollEvents();
  }

  void SceneBase::Render(double diff) {
    CHECK_GL();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    { // g-buffer pass
      
      CHECK_GL_HERE();
      js_GBufferPass->Begin();
      js_GBufferPass->UpdateProjection(js_World->GetProjection());
      js_GBufferPass->UpdateView(js_World->GetView());
      js_World->RenderChunks(diff);
      js_GBufferPass->End();
      CHECK_GL_HERE();
    }

    { // SSAO
      CHECK_GL_HERE();
      glBindFramebuffer(GL_FRAMEBUFFER, ssao_.framebuffer);
      js_World->RenderSSAOPass(diff, js_GBufferPass->textures_.position, js_GBufferPass->textures_.normal, ssao_.noise_texture);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      CHECK_GL_HERE();
    }

    { // SSAO blur
      CHECK_GL_HERE();
      glBindFramebuffer(GL_FRAMEBUFFER, ssao_blur_.framebuffer);
      js_World->RenderSSAOBlurPass(diff, ssao_.color_texture);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      CHECK_GL_HERE();
    }

    { // deferred lighting pass
      js_World->RenderDeferredLightingPass(diff, js_GBufferPass->textures_.position, js_GBufferPass->textures_.normal, js_GBufferPass->textures_.color, ssao_blur_.color_texture);
    }

    { // skybox
      js_World->RenderSkybox(diff);
    }

    // glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer_.framebuffer);
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // glBlitFramebuffer(0, 0, buffer_size_.x, buffer_size_.y, 0, 0, buffer_size_.x, buffer_size_.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    js_Window->SwapBuffers();
  }

  void SceneBase::UpdateViewport() {
    if (js_Window.IsEmpty()) {
      return;
    }

    glm::ivec2 size = js_Window->GetSize();
    UpdateViewport(size.x, size.y);
  }

  void SceneBase::UpdateViewport(int width, int height) {
    glViewport(0, 0, width, height);
    if (!js_World.IsEmpty()) {
      js_World->UpdateViewportRatio(width, height);
    }

    DeleteGLBuffers();
    InitializeGLBuffers(width, height);
  }
  
  void SceneBase::InitializeGLBuffers(int width, int height) {
    
    buffer_size_.x = width;
    buffer_size_.y = height;

    js_GBufferPass->UpdateBufferSize(width, height);

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

    { // initialize SSAO blur
      glGenFramebuffers(1, &ssao_blur_.framebuffer);
      glBindFramebuffer(GL_FRAMEBUFFER, ssao_blur_.framebuffer);
      glGenTextures(1, &ssao_blur_.color_texture);
      glBindTexture(GL_TEXTURE_2D, ssao_blur_.color_texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_blur_.color_texture, 0);

      assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
      
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
  }

  void SceneBase::DeleteGLBuffers() {
    

    // TODO(daaitch): cleanups missing? ssao, ssao-blur
  }
}