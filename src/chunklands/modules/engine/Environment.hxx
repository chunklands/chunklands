#ifndef __CHUNKLANDS_MODULES_ENGINE_ENVIRONMENT_HXX__
#define __CHUNKLANDS_MODULES_ENGINE_ENVIRONMENT_HXX__

#include <chunklands/js.hxx>

namespace chunklands::modules::engine {

  class Environment : public JSObjectWrap<Environment> {
    JS_IMPL_WRAP(Environment, ONE_ARG({
      JS_CB_STATIC(initialize),
      JS_CB_STATIC(loadProcs),
      JS_CB_STATIC(terminate),
    }))

    JS_DECL_CB_STATIC_VOID(initialize)
    JS_DECL_CB_STATIC_VOID(loadProcs)
    JS_DECL_CB_STATIC_VOID(terminate)
  };

} // namespace chunklands::modules::engine

#endif