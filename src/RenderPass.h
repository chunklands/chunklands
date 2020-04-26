#ifndef __CHUNKLANDS_RENDERPASS_H__
#define __CHUNKLANDS_RENDERPASS_H__

#include "js.h"
#include "GLProgramBase.h"

namespace chunklands {
  class RenderPass {
    JS_ATTR_WRAP(GLProgramBase, Program)

  public:
    virtual void Begin() {
      js_Program->Use();
    }

    virtual void End() {
      js_Program->Unuse();
    }

    virtual void UpdateBufferSize(int /*width*/, int /*height*/) {}
  
  protected:
    virtual void InitializeProgram() {}

    void JSCall_SetProgram(JSCbi info) {
      js_Program = info[0];

      js_Program->Use();
      InitializeProgram();
      js_Program->Unuse();
    }
  };
}

#endif