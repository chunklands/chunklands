#include "js.h"

#include "engine_module.h"
#include "game_module.h"
#include "gl_module.h"
#include "misc_module.h"

#define EXPORT(CLAZZ) exports[#CLAZZ] = CLAZZ::Initialize(env)

namespace chunklands {

  JSObject InitGame(JSEnv env, JSObject exports) {
    using namespace game;
    EXPORT(BlockRegistrarBase);
    EXPORT(ChunkGenerator);
    EXPORT(Scene);
    EXPORT(World);
    return exports;
  }

  JSObject InitMisc(JSEnv env, JSObject exports) {
    using namespace misc;
    EXPORT(Profiler);
    return exports;
  }

  JSObject InitGl(JSEnv env, JSObject exports) {
    using namespace gl;
    EXPORT(ProgramBase);
    return exports;
  }

  JSObject InitEngine(JSEnv env, JSObject exports) {
    using namespace engine;
    EXPORT(Environment);
    EXPORT(GameLoop);
    EXPORT(GBufferPass);
    EXPORT(LightingPass);
    EXPORT(Skybox);
    EXPORT(SkyboxPass);
    EXPORT(SSAOBlurPass);
    EXPORT(SSAOPass);
    EXPORT(Window);
    return exports;
  }
}

using namespace chunklands;

JSObject Init(JSEnv env, JSObject exports) {
  exports["game"]   = InitGame(env, JSObject::New(env));
  exports["misc"]   = InitMisc(env, JSObject::New(env));
  exports["gl"]     = InitGl(env, JSObject::New(env));
  exports["engine"] = InitEngine(env, JSObject::New(env));

  return exports;
}

NODE_API_MODULE(chunklands, Init)
