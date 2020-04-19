#ifndef __CHUNKLANDS_RENDERPASS_H__
#define __CHUNKLANDS_RENDERPASS_H__

#include "napi/PersistentObjectWrap.h"
#include "GLProgramBase.h"

namespace chunklands {
  class RenderPass {
  public:
    void SetProgram(const Napi::Value& program);

    void Begin();
    void End();

  protected:
    virtual void InitializeProgram() {}

  protected:
    NapiExt::PersistentObjectWrap<GLProgramBase> program_;
  };
}

#endif
