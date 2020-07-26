
#include "EngineBridge.hxx"

namespace chunklands::core {

  EngineBridge::EngineBridge(JSCbi info) : JSObjectWrap<EngineBridge>(info) {
    fn_ = JSTSFunction::New(info.Env(), JSFunction::New(info.Env(), [](JSCbi){}, "dummy"), "EngineApiBridgeCallback", 0, 1);
  }

  JS_DEF_INITCTOR(EngineBridge, ONE_ARG({
    JS_CB(terminate),
  }))

  void EngineBridge::JSCall_terminate(JSCbi) {
    fn_.Release();
  }

} // namespace chunklands::core