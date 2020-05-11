
#include "init.h"

#include "engine/engine_module.h"
#include "game/game_module.h"
#include "gl/gl_module.h"
#include "misc/misc_module.h"

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
    return exports;
  }

  JSObject InitGame(JSEnv env, JSObject exports) {
    using namespace modules::game;
    EXPORT(BlockRegistrarBase);
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
