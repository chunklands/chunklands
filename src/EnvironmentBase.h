#ifndef __CHUNKLANDS_ENVIRONMENTBASE_H__
#define __CHUNKLANDS_ENVIRONMENTBASE_H__

#include "js.h"

namespace chunklands {
  class EnvironmentBase : public JSObjectWrap<EnvironmentBase> {
    JS_IMPL_WRAP(EnvironmentBase, ONE_ARG({
      JS_CB_STATIC(initialize),
      JS_CB_STATIC(loadProcs),
      JS_CB_STATIC(terminate),
    }))

    JS_DECL_CB_STATIC_VOID(initialize)
    JS_DECL_CB_STATIC_VOID(loadProcs)
    JS_DECL_CB_STATIC_VOID(terminate)
  };
}

#endif