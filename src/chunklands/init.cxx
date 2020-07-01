
#include "init.hxx"

#include "engine/_.hxx"
#include "gl/_.hxx"
#include "game/_.hxx"
#include "misc/_.hxx"

namespace chunklands {

  JSObject Init(JSEnv env, JSObject exports) {
    exports["engine"] = engine::Init(env, JSObject::New(env));
    exports["game"]   = game::Init(env, JSObject::New(env));
    exports["gl"]     = gl::Init(env, JSObject::New(env));
    exports["misc"]   = misc::Init(env, JSObject::New(env));

    return exports;
  }
}
