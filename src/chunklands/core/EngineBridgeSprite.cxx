
#include "EngineBridge.hxx"
#include "engine_bridge_util.hxx"
#include "resolver.hxx"

namespace chunklands::core {

JSValue
EngineBridge::JSCall_spriteCreate(JSCbi info)
{
    JSObject js_init = info[0].ToObject();

    std::string id = js_init.Get("id").ToString();
    JSArrayBuffer js_data = js_init.Get("data").As<JSArrayBuffer>();
    JSValue js_texture = js_init.Get("texture");

    // texture
    std::vector<unsigned char> texture;
    if (js_texture.IsBuffer()) {
        JSBuffer<unsigned char> js_texture_buffer = js_texture.As<JSBuffer<unsigned char>>();
        texture.resize(js_texture_buffer.ByteLength());
        std::memcpy(texture.data(), js_texture_buffer.Data() + js_texture_buffer.ByteOffset(), js_texture_buffer.ByteLength());
    }

    // data
    const size_t byte_length = js_data.ByteLength();
    JS_ENGINE_CHECK(info.Env(), byte_length % sizeof(engine::CEVaoElementSprite) == 0, JSValue());
    const int items = byte_length / sizeof(engine::CEVaoElementSprite);

    std::vector<engine::CEVaoElementSprite> data;
    data.resize(items);
    std::memcpy(data.data(), js_data.Data(), byte_length);

    engine::CESpriteCreateInit init = {
        .id = std::move(id),
        .data = std::move(data),
        .texture = std::move(texture)
    };

    return MakeEngineCall(info.Env(),
        engine_->SpriteCreate(std::move(init)),
        create_resolver<engine::CESpriteHandle*>(handle_resolver<engine::CESpriteHandle*>));
}

} // namespace chunklands::core