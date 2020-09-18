
#include "EngineBridge.hxx"

namespace chunklands::core {

JSValue
EngineBridge::JSCall_gameOn(JSCbi info)
{
    return OpenGLThreadEventHandler<engine::CEGameEvent>(
        info.Env(), info[0], info[1],
        [this](const std::string& type, auto&& cb) { return engine_->GameOn(type, std::move(cb)); },
        [](const engine::CEGameEvent& event, JSEnv env, JSObject js_event) {
            if (event.type == "pointingblockchange") {
                if (event.pointingblockchange) {
                    JSObject js_pos = JSObject::New(env);
                    js_pos["x"] = event.pointingblockchange->x;
                    js_pos["y"] = event.pointingblockchange->y;
                    js_pos["z"] = event.pointingblockchange->z;

                    js_event["pos"] = js_pos;
                } else {
                    js_event["pos"] = env.Null();
                }
            }
        });
}

} // namespace chunklands::core