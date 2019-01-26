#ifndef __CHUNKLANDS_ENVIRONMENTBASE_H__
#define __CHUNKLANDS_ENVIRONMENTBASE_H__

#include <napi.h>
#include "napi/object_wrap_util.h"

namespace chunklands {
  class EnvironmentBase : public Napi::ObjectWrap<EnvironmentBase> {
    DECLARE_OBJECT_WRAP(EnvironmentBase)
    DECLARE_OBJECT_WRAP_CB(static void Initialize_)
    DECLARE_OBJECT_WRAP_CB(static void LoadProcs)
    DECLARE_OBJECT_WRAP_CB(static void Terminate)
  };
}

#endif