#ifndef __CHUNKLANDS_ENVIRONMENTBASE_H__
#define __CHUNKLANDS_ENVIRONMENTBASE_H__

#include "js.h"

namespace chunklands {
  class EnvironmentBase : public JSWrap<EnvironmentBase> {
    JS_DECL_WRAP(EnvironmentBase)
    JS_DECL_CB_STATIC_VOID(initialize)
    JS_DECL_CB_STATIC_VOID(loadProcs)
    JS_DECL_CB_STATIC_VOID(terminate)
  };
}

#endif