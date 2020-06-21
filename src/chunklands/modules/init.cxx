
#include "init.hxx"

#include "engine/engine_module.hxx"
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
    return exports;
  }

  JSObject InitGame(JSEnv env, JSObject exports) {
    using namespace modules::game;
    EXPORT(BlockRegistrar);
    EXPORT(ChunkGenerator);
    EXPORT(Scene);
    EXPORT(World);
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
