
#include "init.hxx"

#include <iostream>

#include "EngineBridge.hxx"
#include "EngineApiBridge.hxx"

namespace chunklands {

  JSObject Init(JSEnv env, JSObject exports) {
    // std::cout << std::fixed << std::setprecision(DEBUG_FLOAT_PRECISION);

    exports["EngineBridge"] = core::EngineBridge::Initialize(env);
    exports["EngineApiBridge"] = core::EngineApiBridge::Initialize(env);

    return exports;
  }
}
