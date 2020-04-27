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
      js_SSAOPass->Begin();
      js_SSAOPass->UpdateProjection(js_World->GetProjection());
      js_SSAOPass->BindPositionTexture(js_GBufferPass->textures_.position);
      js_SSAOPass->BindNormalTexture(js_GBufferPass->textures_.normal);

      // TODO(daaitch): no js_World call: simply render_quad->Render() instead here
      js_World->RenderSSAOPass(diff);
      js_SSAOPass->End();
      CHECK_GL_HERE();
    }

    { // SSAO blur
      CHECK_GL_HERE();
      js_SSAOBlurPass->Begin();
      js_SSAOBlurPass->BindSSAOTexture(js_SSAOPass->textures_.color);
      js_World->RenderSSAOBlurPass(diff);
      js_SSAOBlurPass->End();
      CHECK_GL_HERE();
    }

    { // deferred lighting pass
      CHECK_GL_HERE();
      js_LightingPass->Begin();
      js_LightingPass->BindPositionTexture(js_GBufferPass->textures_.position);
      js_LightingPass->BindNormalTexture(js_GBufferPass->textures_.normal);
      js_LightingPass->BindColorTexture(js_GBufferPass->textures_.color);
      js_LightingPass->BindSSAOTexture(js_SSAOBlurPass->textures_.color);

      js_LightingPass->UpdateRenderDistance(((float)js_World->GetRenderDistance() - 0.5f) * Chunk::SIZE);

      glm::vec3 sun_position = glm::normalize(glm::mat3(js_World->GetView()) * glm::vec3(-3, 1, 3));
      js_LightingPass->UpdateSunPosition(sun_position);
      js_World->RenderDeferredLightingPass(diff);
      js_LightingPass->End();
      CHECK_GL_HERE();
    }

    { // skybox
      CHECK_GL_HERE();
      js_SkyboxPass->Begin();
      js_SkyboxPass->BindSkyboxTexture(js_GBufferPass->textures_.position);
      js_SkyboxPass->UpdateProjection(js_World->GetProjection());
      js_SkyboxPass->UpdateView(js_World->GetViewSkybox());
      js_World->RenderSkybox(diff);
      js_SkyboxPass->End();
      CHECK_GL_HERE();
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

    InitializeGLBuffers(width, height);
  }
  
  void SceneBase::InitializeGLBuffers(int width, int height) {
    
    buffer_size_.x = width;
    buffer_size_.y = height;

    js_GBufferPass->UpdateBufferSize(width, height);
    js_SSAOPass->UpdateBufferSize(width, height);
    js_SSAOBlurPass->UpdateBufferSize(width, height);
    js_LightingPass->UpdateBufferSize(width, height);
  }
}