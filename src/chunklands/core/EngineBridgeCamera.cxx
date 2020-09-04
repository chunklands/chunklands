
#include "EngineBridge.hxx"
#include "engine_bridge_util.hxx"
#include "resolver.hxx"

namespace chunklands::core {

JSValue
EngineBridge::JSCall_cameraAttachWindow(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&handle, info.Env(), info[0]), JSValue());

    return MakeEngineCall(info.Env(),
        engine_->CameraAttachWindow(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_cameraDetachWindow(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&handle, info.Env(), info[0]), JSValue());

    return MakeEngineCall(info.Env(),
        engine_->CameraDetachWindow(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_cameraGetPosition(JSCbi info)
{
    return MakeEngineCall(info.Env(),
        engine_->CameraGetPosition(),
        create_resolver<engine::CECameraPosition>([](JSEnv env, const engine::CECameraPosition& pos) {
            JSObject js_result = JSObject::New(env);
            js_result["x"] = JSNumber::New(env, pos.x);
            js_result["y"] = JSNumber::New(env, pos.y);
            js_result["z"] = JSNumber::New(env, pos.z);

            return js_result;
        }));
}

JSValue
EngineBridge::JSCall_cameraOn(JSCbi info)
{
    return EventHandler<engine::CECameraEvent>(
        info.Env(), info[0], info[1], [this](const std::string& type, auto cb) { return engine_->CameraOn(type, std::move(cb)); }, [](const engine::CECameraEvent& event, JSEnv, JSObject js_event) {
      if (event.type == "positionchange") {
        js_event["x"] = event.positionchange.x;
        js_event["y"] = event.positionchange.y;
        js_event["z"] = event.positionchange.z;
      } });
}

} // namespace chunklands::core