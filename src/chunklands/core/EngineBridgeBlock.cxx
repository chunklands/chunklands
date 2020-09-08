
#include "EngineBridge.hxx"
#include "engine_bridge_util.hxx"
#include "resolver.hxx"

namespace chunklands::core {

engine::FaceType face_type_by_string(const std::string& type)
{
    if (type == "left") {
        return engine::FaceType::Left;
    }

    if (type == "right") {
        return engine::FaceType::Right;
    }

    if (type == "top") {
        return engine::FaceType::Top;
    }

    if (type == "bottom") {
        return engine::FaceType::Bottom;
    }

    if (type == "front") {
        return engine::FaceType::Front;
    }

    if (type == "back") {
        return engine::FaceType::Back;
    }

    return engine::FaceType::Unknown;
}

JSValue
EngineBridge::JSCall_blockCreate(JSCbi info)
{
    JSObject js_init = info[0].ToObject();

    std::string id = js_init.Get("id").ToString();
    bool opaque = js_init.Get("opaque").ToBoolean();
    JSObject js_faces = js_init.Get("faces").ToObject();
    JSValue js_texture = js_init.Get("texture");

    // texture
    std::vector<unsigned char> texture;
    if (js_texture.IsBuffer()) {
        JSBuffer<unsigned char> js_texture_buffer = js_texture.As<JSBuffer<unsigned char>>();
        texture.resize(js_texture_buffer.ByteLength());
        std::memcpy(texture.data(), js_texture_buffer.Data() + js_texture_buffer.ByteOffset(), js_texture_buffer.ByteLength());
    }

    // faces
    std::vector<engine::CEBlockFace> faces;

    const JSArray js_faces_names = js_faces.GetPropertyNames();
    const uint32_t face_names_length = js_faces_names.Length();
    faces.resize(face_names_length);

    for (uint32_t i = 0; i < face_names_length; i++) {
        JSValue js_face_name = js_faces_names.Get(i);
        const std::string face_name = js_face_name.ToString();
        JSArrayBuffer js_data = js_faces.Get(js_face_name).As<JSArrayBuffer>();

        const size_t byte_length = js_data.ByteLength();
        JS_ENGINE_CHECK(info.Env(), byte_length % sizeof(engine::CEVaoElementChunkBlock) == 0, JSValue());
        const int items = byte_length / sizeof(engine::CEVaoElementChunkBlock);

        std::vector<engine::CEVaoElementChunkBlock> data;
        data.resize(items);
        std::memcpy(data.data(), js_data.Data(), byte_length);

        faces.push_back({ .type = face_type_by_string(face_name),
            .data = std::move(data) });
    }

    engine::CEBlockCreateInit init = {
        .id = std::move(id),
        .opaque = opaque,
        .faces = std::move(faces),
        .texture = std::move(texture)
    };

    return MakeEngineCall(info.Env(),
        engine_->BlockCreate(std::move(init)),
        create_resolver<engine::CEBlockHandle*>(handle_resolver<engine::CEBlockHandle*>));
}

JSValue
EngineBridge::JSCall_blockBake(JSCbi info)
{
    return MakeEngineCall(info.Env(),
        engine_->BlockBake(),
        create_resolver<engine::CEBlockBakeResult>([](JSEnv env, const engine::CEBlockBakeResult& result) {
            JSObject js_sprites = JSObject::New(env);
            for (auto&& [id, sprite] : result.sprites) {
                js_sprites[id] = get_handle(env, sprite);
            }

            JSObject js_blocks = JSObject::New(env);
            for (auto&& [id, block] : result.blocks) {
                js_blocks[id] = get_handle(env, block);
            }

            JSObject js_result = JSObject::New(env);
            js_result["sprites"] = js_sprites;
            js_result["blocks"] = js_blocks;

            return js_result;
        }));
}
} // namespace chunklands::core