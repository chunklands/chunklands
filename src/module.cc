#include <napi.h>
#include "gl.h"

#include "EnvironmentBase.h"
#include "GameLoopBase.h"
#include "SceneBase.h"
#include "WindowBase.h"

#define EXPORTS_CLASS(clazz) \
  do { \
    clazz::Initialize(env); \
    exports[#clazz] = clazz::constructor.Value(); \
  } while (0)

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    
  using namespace chunklands;

  EXPORTS_CLASS(EnvironmentBase);
  EXPORTS_CLASS(GameLoopBase);
  EXPORTS_CLASS(SceneBase);
  EXPORTS_CLASS(WindowBase);

  return exports;
}

NODE_API_MODULE(chunklands_core, Init)