
#include "EngineBridge.hxx"
#include "resolver.hxx"

namespace chunklands::core {

JSValue
EngineBridge::JSCall_GLFWInit(JSCbi info)
{
    return MakeEngineCall(info.Env(),
        engine_->GLFWInit(),
        create_resolver<engine::CENone>());
}

void EngineBridge::JSCall_GLFWPollEvents(JSCbi info)
{
    resolve(info.Env(), engine_->GLFWPollEvents());
}

} // namespace chunklands::core