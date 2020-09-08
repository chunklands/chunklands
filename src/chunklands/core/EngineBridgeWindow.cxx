
#include "EngineBridge.hxx"
#include "engine_bridge_util.hxx"
#include "resolver.hxx"

namespace chunklands::core {

JSValue
EngineBridge::JSCall_windowCreate(JSCbi info)
{
    const int width = info[0].ToNumber();
    const int height = info[1].ToNumber();
    std::string title = info[2].ToString();

    return MakeEngineCall(info.Env(),
        engine_->WindowCreate(width, height, std::move(title)),
        create_resolver<engine::CEWindowHandle*>(handle_resolver<engine::CEWindowHandle*>));
}

JSValue
EngineBridge::JSCall_windowLoadGL(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&handle, info.Env(), info[0]), JSValue());
    return MakeEngineCall(info.Env(),
        engine_->WindowLoadGL(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_windowOn(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&handle, info.Env(), info[0]), JSValue());

    return EventHandler<engine::CEWindowEvent>(
        info.Env(), info[1], info[2], [this, handle](const std::string& type, auto&& cb) { return engine_->WindowOn(handle, type, std::move(cb)); }, [](const engine::CEWindowEvent& event, JSEnv env, JSObject js_event) {
      if (event.type == "shouldclose") {
        // nothing
      } else if (event.type == "click") {
        js_event["button"]  = JSNumber::New(env, event.click.button);
        js_event["action"]  = JSNumber::New(env, event.click.action);
        js_event["mods"]    = JSNumber::New(env, event.click.mods);
      } else if (event.type == "key") {
        js_event["key"]       = JSNumber::New(env, event.key.key);
        js_event["scancode"]  = JSNumber::New(env, event.key.scancode);
        js_event["action"]    = JSNumber::New(env, event.key.action);
        js_event["mods"]      = JSNumber::New(env, event.key.mods);
      } else if (event.type == "resize") {
        js_event["width"]   = JSNumber::New(env, event.resize.width);
        js_event["height"]  = JSNumber::New(env, event.resize.height);
      } });
}

JSValue
EngineBridge::JSCall_windowGetSize(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&handle, info.Env(), info[0]), JSValue());

    return resolve(info.Env(), engine_->WindowGetSize(handle), [&](const engine::CESize2i& size) {
        JSObject js_size = JSObject::New(info.Env());
        js_size["width"] = JSNumber::New(info.Env(), size.width);
        js_size["height"] = JSNumber::New(info.Env(), size.height);

        return js_size;
    });
}
} // namespace chunklands::core