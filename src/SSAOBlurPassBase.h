#ifndef __CHUNKLANDS_SSAOBLURPASSBASE_H__
#define __CHUNKLANDS_SSAOBLURPASSBASE_H__

#include "js.h"
#include "ARenderPass.h"

namespace chunklands {
  class SSAOBlurPassBase : public JSWrap<SSAOBlurPassBase>, public ARenderPass {
    JS_DECL_WRAP(SSAOBlurPassBase)

  protected:
    void InitializeProgram();

  public:
    void BindSSAOTexture(GLuint texture);
  };
}

#endif
