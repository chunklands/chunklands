
#include "EngineBridge.hxx"
#include "engine_bridge_util.hxx"
#include "resolver.hxx"

namespace chunklands::core {

JSValue
EngineBridge::JSCall_renderPipelineInit(JSCbi info)
{
    engine::CEWindowHandle* handle = nullptr;
    JS_ENGINE_CHECK(unsafe_get_handle_ptr(&handle, info.Env(), info[0]), info.Env(), JSValue());

    engine::CERenderPipelineInit init;
    JSObject js_init = info[1].ToObject();

    JSObject js_gbuffer = js_init.Get("gbuffer").ToObject();
    init.gbuffer.vertex_shader = js_gbuffer.Get("vertexShader").ToString();
    init.gbuffer.fragment_shader = js_gbuffer.Get("fragmentShader").ToString();

    JSObject js_lighting = js_init.Get("lighting").ToObject();
    init.lighting.vertex_shader = js_lighting.Get("vertexShader").ToString();
    init.lighting.fragment_shader = js_lighting.Get("fragmentShader").ToString();

    return MakeEngineCall(info.Env(),
        engine_->RenderPipelineInit(handle, std::move(init)),
        create_resolver<engine::CENone>());
}

} // namespace chunklands::core