
#include "init.hxx"

#include "engine/Environment.hxx"
#include "engine/GameLoop.hxx"
#include "engine/GBufferPass.hxx"
#include "engine/LightingPass.hxx"
#include "engine/Skybox.hxx"
#include "engine/SkyboxPass.hxx"
#include "engine/SSAOBlurPass.hxx"
#include "engine/SSAOPass.hxx"
#include "engine/Window.hxx"
#include "engine/Camera.hxx"
#include "engine/MovementController.hxx"
#include "engine/FontLoader.hxx"
#include "engine/TextRenderer.hxx"
#include "engine/GameOverlayRenderer.hxx"
#include "engine/BlockSelectPass.hxx"
#include "game/game_module.hxx"
#include "gl/gl_module.hxx"
#include "misc/misc_module.hxx"

#define EXPORT(CLAZZ) exports[#CLAZZ] = CLAZZ::Initialize(env)

namespace chunklands::modules {

  JSObject InitEngine(JSEnv env, JSObject exports) {
    using namespace modules::engine;
    EXPORT(Environment);
    EXPORT(GameLoop);
    EXPORT(GBufferPass);
    EXPORT(LightingPass);
    EXPORT(Skybox);
    EXPORT(SkyboxPass);
    EXPORT(SSAOBlurPass);
    EXPORT(SSAOPass);
    EXPORT(Window);
    EXPORT(Camera);
    EXPORT(MovementController);
    EXPORT(FontLoader);
    EXPORT(TextRenderer);
    EXPORT(GameOverlayRenderer);
    EXPORT(BlockSelectPass);
    return exports;
  }

  JSObject InitGame(JSEnv env, JSObject exports) {
    using namespace modules::game;
    EXPORT(BlockRegistrar);
    EXPORT(SpriteRegistrar);
    EXPORT(ChunkGenerator);
    EXPORT(Scene);
    EXPORT(World);
    EXPORT(GameOverlay);
    return exports;
  }

  JSObject InitGl(JSEnv env, JSObject exports) {
    using namespace modules::gl;
    EXPORT(ProgramBase);
    return exports;
  }

  JSObject InitMisc(JSEnv env, JSObject exports) {
    using namespace modules::misc;
    EXPORT(Profiler);
    return exports;
  }
  

  JSObject Init(JSEnv env, JSObject exports) {
    exports["engine"] = InitEngine(env, JSObject::New(env));
    exports["game"]   = InitGame(env, JSObject::New(env));
    exports["gl"]     = InitGl(env, JSObject::New(env));
    exports["misc"]   = InitMisc(env, JSObject::New(env));

    return exports;
  }
}
