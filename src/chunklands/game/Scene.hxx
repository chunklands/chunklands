#ifndef __CHUNKLANDS_GAME_SCENE_HXX__
#define __CHUNKLANDS_GAME_SCENE_HXX__

#include <chunklands/js.hxx>
#include <chunklands/engine/IScene.hxx>
#include <chunklands/engine/Window.hxx>
#include "BlockRegistrar.hxx"
#include <chunklands/engine/GBufferPass.hxx>
#include <chunklands/engine/SSAOPass.hxx>
#include <chunklands/engine/SSAOBlurPass.hxx>
#include <chunklands/engine/LightingPass.hxx>
#include <chunklands/engine/SkyboxPass.hxx>
#include <chunklands/engine/TextRenderer.hxx>
#include <chunklands/engine/GameOverlayRenderer.hxx>
#include <chunklands/engine/BlockSelectPass.hxx>
#include <chunklands/engine/Skybox.hxx>
#include <chunklands/engine/Camera.hxx>
#include <chunklands/engine/MovementController.hxx>
#include "GameOverlay.hxx"
#include "World.hxx"
#include <chunklands/gl/RenderQuad.hxx>
#include <chunklands/gl/Texture2.hxx>

namespace chunklands::game {

  class Scene : public JSObjectWrap<Scene>, public engine::IScene {
    JS_IMPL_WRAP(Scene, ONE_ARG({
      JS_SETTER(Window),
      JS_SETTER(World),
      JS_SETTER(ModelTexture),
      JS_SETTER(GBufferPass),
      JS_SETTER(SSAOPass),
      JS_SETTER(SSAOBlurPass),
      JS_SETTER(LightingPass),
      JS_SETTER(SkyboxPass),
      JS_SETTER(TextRenderer),
      JS_SETTER(GameOverlayRenderer),
      JS_SETTER(BlockSelectPass),
      JS_SETTER(Skybox),
      JS_SETTER(Camera),
      JS_ABSTRACT_WRAP(engine::IScene, IScene),
      JS_SETTER(MovementController),
      JS_SETTER(GameOverlay),
      JS_SETTER(FlightMode),
      JS_GETTER(FlightMode),
      JS_SETTER(SSAO),
      JS_GETTER(SSAO),
    }))

    JS_DECL_SETTER_WRAP(engine::Window, Window)
    JS_DECL_SETTER_WRAP(World, World)
    JS_IMPL_SETTER_WRAP(gl::Texture2, ModelTexture)
    JS_IMPL_SETTER_WRAP(engine::GBufferPass, GBufferPass)
    JS_IMPL_SETTER_WRAP(engine::SSAOPass, SSAOPass)
    JS_IMPL_SETTER_WRAP(engine::SSAOBlurPass, SSAOBlurPass)
    JS_IMPL_SETTER_WRAP(engine::LightingPass, LightingPass)
    JS_IMPL_SETTER_WRAP(engine::SkyboxPass, SkyboxPass)
    JS_IMPL_SETTER_WRAP(engine::TextRenderer, TextRenderer)
    JS_IMPL_SETTER_WRAP(engine::GameOverlayRenderer, GameOverlayRenderer)
    JS_IMPL_SETTER_WRAP(engine::BlockSelectPass, BlockSelectPass)
    JS_IMPL_SETTER_WRAP(engine::Skybox, Skybox)
    JS_IMPL_SETTER_WRAP(engine::Camera, Camera)
    JS_IMPL_ABSTRACT_WRAP(engine::IScene, IScene)
    JS_IMPL_SETTER_WRAP(engine::MovementController, MovementController)
    JS_IMPL_SETTER_WRAP(GameOverlay, GameOverlay)
    
    JSValue JSCall_GetFlightMode(JSCbi info) {
      return JSBoolean::New(info.Env(), flight_mode_);
    }

    void JSCall_SetFlightMode(JSCbi info) {
      flight_mode_ = info[0].ToBoolean();
    }

    JSValue JSCall_GetSSAO(JSCbi info) {
      return JSBoolean::New(info.Env(), ssao_);
    }

    void JSCall_SetSSAO(JSCbi info) {
      ssao_ = info[0].ToBoolean();
    }
  
  public:
    virtual void Prepare() override;

    virtual void Update(double diff) override;
    virtual void Render(double diff) override;

    void UpdateViewport();
    void UpdateViewport(int width, int height);

  private:
    void InitializeGLBuffers(int width, int height);

  private:
    boost::signals2::scoped_connection window_on_resize_conn_;
    boost::signals2::scoped_connection window_on_cursor_move_conn_;

    glm::ivec2 buffer_size_;

    gl::RenderQuad render_quad_;

    bool flight_mode_ = true;
    float vy_ = 0.f;

    bool ssao_ = true;

    std::optional<glm::ivec3> pointing_block_;

    GLuint  render_gbuffer_query_ = 0,
            render_ssao_query_ = 0,
            render_ssaoblur_query_ = 0,
            render_lighting_query_ = 0,
            render_skybox_query_ = 0,
            render_text_query_ = 0;

    JSObjectRef js_Events;
  };

} // namespace chunklands::game

#endif