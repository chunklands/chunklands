
#include "EngineBridge.hxx"
#include "engine_bridge_util.hxx"
#include "handle.hxx"
#include "resolver.hxx"

namespace chunklands::core {

JSValue EngineBridge::JSCall_textCreate(JSCbi info)
{
    engine::CEFontHandle* font = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&font, info.Env(), info[0]), JSValue());
    return MakeEngineCall(info.Env(), engine_->TextCreate(font),
        create_resolver<engine::CETextHandle*>(handle_resolver<engine::CETextHandle*>));
}

JSValue EngineBridge::JSCall_textUpdate(JSCbi info)
{
    engine::CETextHandle* handle = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&handle, info.Env(), info[0]), JSValue());

    JSObject js_update = info[1].ToObject();

    engine::CETextUpdate update;
    if (JSValue js_pos_value = js_update.Get("pos"); !js_pos_value.IsUndefined()) {
        JSObject js_pos = js_pos_value.ToObject();
        update.pos = engine::CEVector2f {
            .x = js_pos.Get("x").ToNumber(),
            .y = js_pos.Get("y").ToNumber()
        };
    }

    if (JSValue js_text_value = js_update.Get("text"); !js_text_value.IsUndefined()) {
        update.text = js_text_value.ToString();
    }

    return MakeEngineCall(info.Env(), engine_->TextUpdate(handle, std::move(update)),
        create_resolver<engine::CENone>());
}

} // namespace chunklands::core