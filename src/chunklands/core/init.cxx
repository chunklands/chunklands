
#include "init.hxx"

// #include "engine/_.hxx"
// #include "gl/_.hxx"
// #include "game/_.hxx"
// #include "misc/_.hxx"
#include <iostream>
// #include <iomanip>
// #include "debug.hxx"
#include <chunklands/core/EngineBridge.hxx>

namespace chunklands {

  JSObject Init(JSEnv env, JSObject exports) {
    // std::cout << std::fixed << std::setprecision(DEBUG_FLOAT_PRECISION);

    // exports["engine"] = engine::Init(env, JSObject::New(env));
    // exports["game"]   = game::Init(env, JSObject::New(env));
    // exports["gl"]     = gl::Init(env, JSObject::New(env));
    // exports["misc"]   = misc::Init(env, JSObject::New(env));
    exports["EngineBridge"] = core::EngineBridge::Initialize(env);

    return exports;
  }
}
