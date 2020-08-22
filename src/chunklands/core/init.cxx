
#include "init.hxx"

#include <iostream>

#include "EngineBridge.hxx"

#include <chunklands/libcxx/easylogging++.hxx>

namespace chunklands {

  JSObject Init(JSEnv env, JSObject exports) {

    LOG_PROCESS("initialize NAPI module");

    exports["EngineBridge"] = core::EngineBridge::Initialize(env);

    return exports;
  }
}
