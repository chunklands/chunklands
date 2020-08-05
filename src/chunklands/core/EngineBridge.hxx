#ifndef __CHUNKLANDS_CORE_ENGINE_BRIDGE_HXX__
#define __CHUNKLANDS_CORE_ENGINE_BRIDGE_HXX__

#include "js.hxx"
#include <chunklands/engine/Engine.hxx>

namespace chunklands::core {

  class EngineBridge : public JSObjectWrap<EngineBridge> {
    JS_DECL_INITCTOR()

    JS_DECL_CB_VOID(terminate)
    JS_DECL_CB_VOID(startProfiling)
    JS_DECL_CB_VOID(stopProfiling)

  public:
    EngineBridge(JSCbi info);

    engine::Engine& GetEngine() {
      return engine_;
    }

    JSTSFunction& GetJSThreadSafeFunction() {
      return fn_;
    }

  private:
    engine::Engine engine_;
    JSTSFunction fn_;
  };

} // namespace chunklands::core

#endif