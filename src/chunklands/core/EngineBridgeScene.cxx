
#include "EngineBridge.hxx"
#include "engine_bridge_util.hxx"
#include "resolver.hxx"

namespace chunklands::core {

JSValue
EngineBridge::JSCall_sceneAddChunk(JSCbi info)
{
    engine::CEChunkHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());

    return MakeEngineCall(info.Env(),
        engine_->SceneAddChunk(handle),
        create_resolver<engine::CENone>());
}

JSValue
EngineBridge::JSCall_sceneRemoveChunk(JSCbi info)
{
    engine::CEChunkHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());

    return MakeEngineCall(info.Env(),
        engine_->SceneRemoveChunk(handle),
        create_resolver<engine::CENone>());
}
} // namespace chunklands::core