
#include "_.hxx"

#include "Environment.hxx"
#include "GameLoop.hxx"
#include "GBufferPass.hxx"
#include "LightingPass.hxx"
#include "Skybox.hxx"
#include "SkyboxPass.hxx"
#include "SSAOBlurPass.hxx"
#include "SSAOPass.hxx"
#include "Window.hxx"
#include "Camera.hxx"
#include "MovementController.hxx"
#include "FontLoader.hxx"
#include "TextRenderer.hxx"
#include "GameOverlayRenderer.hxx"
#include "BlockSelectPass.hxx"

#include <chunklands/jsexport.hxx>

namespace chunklands::engine {

  JSObject Init(JSEnv env, JSObject exports) {
    JS_EXPORT(Environment);
    JS_EXPORT(GameLoop);
    JS_EXPORT(GBufferPass);
    JS_EXPORT(LightingPass);
    JS_EXPORT(Skybox);
    JS_EXPORT(SkyboxPass);
    JS_EXPORT(SSAOBlurPass);
    JS_EXPORT(SSAOPass);
    JS_EXPORT(Window);
    JS_EXPORT(Camera);
    JS_EXPORT(MovementController);
    JS_EXPORT(FontLoader);
    JS_EXPORT(TextRenderer);
    JS_EXPORT(GameOverlayRenderer);
    JS_EXPORT(BlockSelectPass);
    return exports;
  }

} // namespace chunklands::engine