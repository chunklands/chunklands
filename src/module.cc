#include <napi.h>

#include "EnvironmentBase.h"
#include "GameLoopBase.h"
#include "napi/object_wrap_util.h"
#include "SceneBase.h"
#include "WindowBase.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    
  using namespace chunklands;
  EXPORTS_OBJECT_WRAP(env, EnvironmentBase);
  EXPORTS_OBJECT_WRAP(env, GameLoopBase);
  EXPORTS_OBJECT_WRAP(env, SceneBase);
  EXPORTS_OBJECT_WRAP(env, WindowBase);

  return exports;
}

NODE_API_MODULE(chunklands, Init)