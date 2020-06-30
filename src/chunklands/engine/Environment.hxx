#ifndef __CHUNKLANDS_ENGINE_ENVIRONMENT_HXX__
#define __CHUNKLANDS_ENGINE_ENVIRONMENT_HXX__

#include <chunklands/js.hxx>

namespace chunklands::engine {

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

} // namespace chunklands::engine

#endif