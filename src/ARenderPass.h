#ifndef __CHUNKLANDS_ARENDERPASS_H__
#define __CHUNKLANDS_ARENDERPASS_H__

#include "js.h"
#include "GLProgramBase.h"

namespace chunklands {
  class ARenderPass {
    JS_DECL_SETTER_REF(GLProgramBase, Program)

  public:
    void Begin() {
      js_Program->Use();
    }

    void End() {
      js_Program->Unuse();
    }
  
  protected:
    virtual void InitializeProgram() {}
  };
}

#endif