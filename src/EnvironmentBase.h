#ifndef __CHUNKLANDS_ENVIRONMENTBASE_H__
#define __CHUNKLANDS_ENVIRONMENTBASE_H__

#include <napi.h>

namespace chunklands {
  class EnvironmentBase : public Napi::ObjectWrap<EnvironmentBase> {
  public:
    static Napi::FunctionReference constructor;
    static void Initialize(Napi::Env env);

  public:
    EnvironmentBase(const Napi::CallbackInfo& info);

  public:
    static void Initialize_(const Napi::CallbackInfo& info);
    static void LoadProcs(const Napi::CallbackInfo& info);
    static void Terminate(const Napi::CallbackInfo& info);
  };
}

#endif