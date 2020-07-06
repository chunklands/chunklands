
#include "Scene.hxx"
#include <chunklands/jsmath.hxx>
#include <chunklands/misc/Profiler.hxx>

namespace chunklands::game {


  JS_DEF_WRAP(Scene)

  void Scene::JSCall_SetWindow(JSCbi info) {
    js_Window = info[0].ToObject();
    UpdateViewport();

    window_on_resize_conn_ = js_Window->on_resize.connect([this](int width, int height) {
      UpdateViewport(width, height);
    });

    window_on_cursor_move_conn_ = js_Window->on_game_control_look.connect([this](double xdiff, double ydiff) {
      const float yaw_rad   = .002 * xdiff;
      const float pitch_rad = .002 * ydiff;

      js_Camera->AddLook(yaw_rad, pitch_rad);
    });
  }

  void Scene::JSCall_SetWorld(JSCbi info) {
    js_World = info[0].ToObject();
    UpdateViewport();
  }

  void Scene::JSCall_jump(JSCbi) {
    vy_ = -10.f;
  }

  void Scene::Prepare() {
    // depth test
    glEnable(GL_DEPTH_TEST);

    // culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    // TODO(daaitch): delete
    glGenQueries(1, &render_gbuffer_query_);
    glGenQueries(1, &render_ssao_query_);
    glGenQueries(1, &render_ssaoblur_query_);
    glGenQueries(1, &render_lighting_query_);
    glGenQueries(1, &render_skybox_query_);
    glGenQueries(1, &render_text_query_);

    // world
    js_World->Prepare();
    js_GameOverlay->Prepare();

    js_Events = Napi::Persistent(Value().Get("events").ToObject());
  }

  void Scene::Update(double diff) {
    auto&& look = js_Camera->GetLook();

    math::fvec3 movement {0.f, 0.f, 0.f};

    if (flight_mode_) {
      constexpr float move_factor = 20.f;

      if (js_Window->GetKey(GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 move(-sinf(look.x) * cosf(look.y),
                      sinf(look.y),
                      -cosf(look.x) * cosf(look.y));
        movement += move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 move(-sinf(look.x) * cosf(look.y),
                      sinf(look.y),
                      -cosf(look.x) * cosf(look.y));
        movement += -move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 move(-cosf(look.x) * cosf(look.y),
                      0.f,
                      sinf(look.x) * cosf(look.y));
        movement += move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 move(-cosf(look.x) * cosf(look.y),
                      0.f,
                      sinf(look.x) * cosf(look.y));
        movement += -move_factor * (float)diff * move;
      }
    } else {
      constexpr float move_factor = 5.f;

      vy_ += 25.f * diff;
      movement.y -= vy_ * diff;

      if (js_Window->GetKey(GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 move(-sinf(look.x),
                      0,
                      -cosf(look.x));
        movement += move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 move(-sinf(look.x),
                      0,
                      -cosf(look.x));
        movement += -move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 move(-cosf(look.x),
                      0.f,
                      sinf(look.x));
        movement += move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 move(-cosf(look.x),
                      0.f,
                      sinf(look.x));
        movement += -move_factor * (float)diff * move;
      }

    }

    int axis = js_MovementController->AddMovement(movement);
    if (axis & math::CollisionAxis::kY) {
      vy_ = 0.f;
    }

    js_Camera->Update(diff);
    js_World->Update(diff, *js_Camera);
    js_GameOverlay->Update(diff);

    {
      auto&& pos = js_Camera->GetPosition();
      auto&& look = js_Camera->GetLook();

      glm::vec3 look_center(-sinf(look.x) * cosf(look.y),
                             sinf(look.y),
                            -cosf(look.x) * cosf(look.y));

      auto&& new_pointing_block = js_World->FindPointingBlock(math::fLine3::from_range(pos, pos + look_center * 2.f));
      if (new_pointing_block != pointing_block_) {
        pointing_block_ = new_pointing_block;

        JSValue value = pointing_block_ ? jsmath::vec3(js_Events.Env(), pointing_block_.value()) : Env().Undefined();

        js_Events.Get("emit").As<JSFunction>().Call(js_Events.Value(), { JSString::New(Env(), "point"), value });
        
        if (pointing_block_) {
          auto&& def = js_World->GetBlock(pointing_block_.value());
          if (def) {
            js_BlockSelectPass->UpdateBlock(def->GetFacesVertexData());
          }
        }
      }
    }
  }

  void Scene::Render(double diff) {
    CHECK_GL();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    { // g-buffer pass
      CHECK_GL_HERE();
      glBeginQuery(GL_TIME_ELAPSED, render_gbuffer_query_);
      js_GBufferPass->Begin();
      js_GBufferPass->UpdateProjection(js_Camera->GetProjection());
      js_GBufferPass->UpdateView(js_Camera->GetView());
      js_ModelTexture->ActiveAndBind(GL_TEXTURE0);
      js_World->Render(diff, *js_Camera);
      js_GBufferPass->End();
      glEndQuery(GL_TIME_ELAPSED);
      CHECK_GL_HERE();
    }

    if (ssao_) { // SSAO
      {
        CHECK_GL_HERE();
        glBeginQuery(GL_TIME_ELAPSED, render_ssao_query_);
        js_SSAOPass->Begin();
        js_SSAOPass->UpdateProjection(js_Camera->GetProjection());
        js_SSAOPass->BindPositionTexture(js_GBufferPass->textures_.position);
        js_SSAOPass->BindNormalTexture(js_GBufferPass->textures_.normal);
        render_quad_.Render();
        js_SSAOPass->End();
        glEndQuery(GL_TIME_ELAPSED);
        CHECK_GL_HERE();
      }

      { // SSAO blur
        CHECK_GL_HERE();
        glBeginQuery(GL_TIME_ELAPSED, render_ssaoblur_query_);
        js_SSAOBlurPass->Begin();
        js_SSAOBlurPass->BindSSAOTexture(js_SSAOPass->textures_.color);
        render_quad_.Render();
        js_SSAOBlurPass->End();
        glEndQuery(GL_TIME_ELAPSED);
        CHECK_GL_HERE();
      }
    }

    { // deferred lighting pass
      CHECK_GL_HERE();
      glBeginQuery(GL_TIME_ELAPSED, render_lighting_query_);
      js_LightingPass->Begin();
      js_LightingPass->BindPositionTexture(js_GBufferPass->textures_.position);
      js_LightingPass->BindNormalTexture(js_GBufferPass->textures_.normal);
      js_LightingPass->BindColorTexture(js_GBufferPass->textures_.color);
      if (ssao_) {
        js_LightingPass->BindSSAOTexture(js_SSAOBlurPass->textures_.color);
      }

      js_LightingPass->UpdateRenderDistance(((float)js_World->GetRenderDistance() - 0.5f) * Chunk::SIZE);

      glm::vec3 sun_position = glm::normalize(glm::mat3(js_Camera->GetView()) * glm::vec3(-3, 10, 3));
      js_LightingPass->UpdateSunPosition(sun_position);
      render_quad_.Render();
      js_LightingPass->End();
      glEndQuery(GL_TIME_ELAPSED);
      CHECK_GL_HERE();
    }

    { // skybox
      CHECK_GL_HERE();
      glBeginQuery(GL_TIME_ELAPSED, render_skybox_query_);
      js_SkyboxPass->Begin();
      js_SkyboxPass->BindSkyboxTexture(js_GBufferPass->textures_.position);
      js_SkyboxPass->UpdateProjection(js_Camera->GetProjection());
      js_SkyboxPass->UpdateView(js_Camera->GetViewSkybox());
      js_Skybox->Render();
      js_SkyboxPass->End();
      glEndQuery(GL_TIME_ELAPSED);
      CHECK_GL_HERE();
    }

    if (pointing_block_) {
      CHECK_GL_HERE();
      js_BlockSelectPass->Begin();
      js_BlockSelectPass->UpdateProjection(js_Camera->GetProjection());
      js_BlockSelectPass->UpdateView(js_Camera->GetView() * glm::translate(glm::mat4(1.f), glm::vec3(pointing_block_.value())));
      js_BlockSelectPass->Render();
      js_BlockSelectPass->End();
      CHECK_GL_HERE();
    }

    { // text
      CHECK_GL_HERE();
      glBeginQuery(GL_TIME_ELAPSED, render_text_query_);
      js_TextRenderer->Begin();
      js_TextRenderer->Render();
      js_TextRenderer->End();
      glEndQuery(GL_TIME_ELAPSED);
      CHECK_GL_HERE();
    }

    { // game overlay
      CHECK_GL_HERE();
      glDisable(GL_CULL_FACE); // TODO(daaitch): culling problem
      js_GameOverlayRenderer->Begin();
      js_ModelTexture->ActiveAndBind(GL_TEXTURE0);
      js_GameOverlayRenderer->UpdateProjection();
      js_GameOverlay->Render(diff);
      js_GameOverlayRenderer->End();
      glEnable(GL_CULL_FACE);
      CHECK_GL_HERE();
    }

    {
      js_Window->SwapBuffers();

      // queries
      GLuint64 result = 0;

      glGetQueryObjectui64v(render_gbuffer_query_, GL_QUERY_RESULT, &result);
      HISTOGRAM_METRIC("render_gbuffer", result / 1000);

      if (ssao_) {
        glGetQueryObjectui64v(render_ssao_query_, GL_QUERY_RESULT, &result);
        HISTOGRAM_METRIC("render_ssao", result / 1000);

        glGetQueryObjectui64v(render_ssaoblur_query_, GL_QUERY_RESULT, &result);
        HISTOGRAM_METRIC("render_ssaoblur", result / 1000);
      }

      glGetQueryObjectui64v(render_lighting_query_, GL_QUERY_RESULT, &result);
      HISTOGRAM_METRIC("render_lighting", result / 1000);

      glGetQueryObjectui64v(render_skybox_query_, GL_QUERY_RESULT, &result);
      HISTOGRAM_METRIC("render_skybox", result / 1000);

      glGetQueryObjectui64v(render_text_query_, GL_QUERY_RESULT, &result);
      HISTOGRAM_METRIC("render_text", result / 1000);
    }
  }

  void Scene::UpdateViewport() {
    if (js_Window.IsEmpty()) {
      return;
    }

    glm::ivec2 size = js_Window->GetSize();
    UpdateViewport(size.x, size.y);
  }

  void Scene::UpdateViewport(int width, int height) {
    glViewport(0, 0, width, height);
    if (!js_Camera.IsEmpty()) {
      js_Camera->UpdateViewportRatio(width, height);
    }

    InitializeGLBuffers(width, height);
  }
  
  void Scene::InitializeGLBuffers(int width, int height) {
    
    buffer_size_.x = width;
    buffer_size_.y = height;

    js_GBufferPass->UpdateBufferSize(width, height);
    if (ssao_) {
      js_SSAOPass->UpdateBufferSize(width, height);
      js_SSAOBlurPass->UpdateBufferSize(width, height);
    }
    js_LightingPass->UpdateBufferSize(width, height);
    js_TextRenderer->UpdateBufferSize(width, height);
    js_GameOverlayRenderer->UpdateBufferSize(width, height);
  }

} // namespace chunklands::game