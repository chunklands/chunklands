
#include "EngineBridge.hxx"
#include "engine_bridge_util.hxx"
#include "resolver.hxx"

namespace chunklands::core {

JSValue
EngineBridge::JSCall_chunkCreate(JSCbi info)
{
    const int x = info[0].ToNumber();
    const int y = info[1].ToNumber();
    const int z = info[2].ToNumber();

    return MakeEngineCall(info.Env(),
        engine_->ChunkCreate(x, y, z),
        create_resolver<engine::CEChunkHandle*>(handle_resolver<engine::CEChunkHandle*>));
}

JSValue
EngineBridge::JSCall_chunkDelete(JSCbi info)
{
    engine::CEChunkHandle* handle = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&handle, info.Env(), info[0]), JSValue());

    return MakeEngineCall(info.Env(),
        engine_->ChunkDelete(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_chunkUpdate(JSCbi info)
{
    engine::CEChunkHandle* handle = nullptr;
    JS_ENGINE_CHECK(info.Env(), unsafe_get_handle_ptr(&handle, info.Env(), info[0]), JSValue());
    JS_ENGINE_CHECK(info.Env(), info[1].IsArrayBuffer(), JSValue());

    JSArrayBuffer js_blocks = info[1].As<JSArrayBuffer>();
    JS_ENGINE_CHECK(info.Env(), js_blocks.ByteLength() == engine::CE_CHUNK_BLOCK_COUNT * sizeof(engine::CEBlockHandle*), JSValue());
    engine::CEBlockHandle** blocks = reinterpret_cast<engine::CEBlockHandle**>(js_blocks.Data());

    napi_ref js_blocks_ref;
    napi_status status = napi_create_reference(info.Env(), js_blocks, 1, &js_blocks_ref);
    NAPI_THROW_IF_FAILED(info.Env(), status, JSValue());

    return MakeEngineCall(info.Env(),
        engine_->ChunkUpdateData(handle, blocks),
        create_resolver<engine::CENone>(),
        [js_blocks_ref, env = info.Env()]() {
            napi_status status = napi_delete_reference(env, js_blocks_ref);
            NAPI_THROW_IF_FAILED(env, status);
        });
}
} // namespace chunklands::core