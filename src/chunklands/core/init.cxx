
#include "init.hxx"

#include <iostream>

#include "EngineBridge.hxx"
#include "EngineApiBridge.hxx"

#include <chunklands/libcxx/easylogging++.hxx>

namespace chunklands {

  JSObject Init(JSEnv env, JSObject exports) {

    LOG_PROCESS("initialize NAPI module");

    exports["EngineBridge"] = core::EngineBridge::Initialize(env);
    exports["EngineApiBridge"] = core::EngineApiBridge::Initialize(env);

    return exports;
  }
}
