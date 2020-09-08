
#include "EngineBridge.hxx"
#include "engine_bridge_util.hxx"
#include "handle.hxx"
#include "resolver.hxx"

namespace chunklands::core {

JSValue EngineBridge::JSCall_spriteInstanceCreate(JSCbi info)
{
    engine::CESpriteHandle* sprite_handle = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&sprite_handle, info.Env(), info[0]), JSValue());
    return MakeEngineCall(info.Env(),
        engine_->SpriteInstanceCreate(sprite_handle),
        create_resolver<engine::CESpriteInstanceHandle*>(handle_resolver<engine::CESpriteInstanceHandle*>));
}

JSValue EngineBridge::JSCall_spriteInstanceUpdate(JSCbi info)
{
    engine::CESpriteInstanceHandle* sprite_instance_handle = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&sprite_instance_handle, info.Env(), info[0]), JSValue());

    JSObject js_update = info[1].ToObject();

    engine::CESpriteInstanceUpdate update;
    if (JSValue js_x = js_update.Get("x"); !js_x.IsUndefined()) {
        update.x = js_x.ToNumber();
    }

    if (JSValue js_y = js_update.Get("y"); !js_y.IsUndefined()) {
        update.y = js_y.ToNumber();
    }

    if (JSValue js_z = js_update.Get("z"); !js_z.IsUndefined()) {
        update.z = js_z.ToNumber();
    }

    if (JSValue js_scale = js_update.Get("scale"); !js_scale.IsUndefined()) {
        update.scale = js_scale.ToNumber();
    }

    if (JSValue js_show = js_update.Get("show"); !js_show.IsUndefined()) {
        update.show = js_show.ToBoolean();
    }

    return MakeEngineCall(info.Env(),
        engine_->SpriteInstanceUpdate(sprite_instance_handle, std::move(update)),
        create_resolver<engine::CENone>());
}

} // namespace chunklands::core