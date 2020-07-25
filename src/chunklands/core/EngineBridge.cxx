
#include "EngineBridge.hxx"

namespace chunklands::core {

  EngineBridge::EngineBridge(JSCbi info) : JSObjectWrap<EngineBridge>(info) {
    engine_ = std::make_unique<engine::Engine>();
  }

  JS_DEF_INITCTOR(EngineBridge, ONE_ARG({
    JS_CB(init),
    JS_CB(shutdown),
  }))

  void EngineBridge::JSCall_init(JSCbi) {
    engine_->Init();
  }

  void EngineBridge::JSCall_shutdown(JSCbi) {
    engine_->Terminate();
  }

} // namespace chunklands::core