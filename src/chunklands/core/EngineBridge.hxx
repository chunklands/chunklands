#ifndef __CHUNKLANDS_CORE_ENGINE_BRIDGE_HXX__
#define __CHUNKLANDS_CORE_ENGINE_BRIDGE_HXX__

#include "js.hxx"
#include <chunklands/engine/Engine.hxx>

namespace chunklands::core {

  class EngineBridge : public JSObjectWrap<EngineBridge> {
    JS_DECL_INITCTOR()

    JS_DECL_CB_VOID(init)
    JS_DECL_CB_VOID(shutdown)

  public:
    EngineBridge(JSCbi info);

    engine::Api* GetApi() {
      return engine_->api.get();
    }

  private:
    std::unique_ptr<engine::Engine> engine_;
  };

} // namespace chunklands::core

#endif