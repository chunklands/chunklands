
#include "EngineBridge.hxx"
#include "resolver.hxx"

namespace chunklands::core {

JSValue EngineBridge::JSCall_fontLoad(JSCbi info)
{
    JSObject js_init = info[0].ToObject();
    engine::CEFontInit init;
    init.name = js_init.Get("name").ToString();
    init.size = js_init.Get("size").ToNumber();

    // TODO(daaitch): check width when loading image in C++
    init.width = js_init.Get("width").ToNumber();
    init.height = js_init.Get("height").ToNumber();

    JSObject js_characters = js_init.Get("characters").As<JSArray>();
    JSArray js_characters_keys = js_characters.GetPropertyNames();
    for (unsigned i = 0; i < js_characters_keys.Length(); i++) {
        JSValue js_key = js_characters_keys.Get(i);
        std::string key = js_key.ToString();
        JSObject item = js_characters.Get(js_key).ToObject();

        int index = int(key[0]);
        auto& ch = init.characters[index];
        ch.x = item.Get("x").ToNumber();
        ch.y = item.Get("y").ToNumber();
        ch.width = item.Get("width").ToNumber();
        ch.height = item.Get("height").ToNumber();
        ch.originX = item.Get("originX").ToNumber();
        ch.originY = item.Get("originY").ToNumber();
        ch.advance = item.Get("advance").ToNumber();
    }

    // texture
    JSBuffer<unsigned char> js_texture = js_init.Get("texture").As<JSBuffer<unsigned char>>();

    init.texture.resize(js_texture.ByteLength());
    std::memcpy(init.texture.data(), js_texture.Data() + js_texture.ByteOffset(), js_texture.ByteLength());

    return MakeEngineCall(info.Env(), engine_->FontLoad(std::move(init)),
        create_resolver<engine::CEFontHandle*>(handle_resolver<engine::CEFontHandle*>));
}

} // namespace chunklands::core