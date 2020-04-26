#ifndef __CHUNKLANDS_SSAOBLURPASSBASE_H__
#define __CHUNKLANDS_SSAOBLURPASSBASE_H__

#include "js.h"
#include "RenderPass.h"

namespace chunklands {
  class SSAOBlurPassBase : public JSObjectWrap<SSAOBlurPassBase>, public RenderPass {
    JS_IMPL_WRAP(SSAOBlurPassBase, ONE_ARG({
      JS_SETTER(Program)
    }))

  protected:
    void InitializeProgram() {
      glUniform1i(js_Program->GetUniformLocation("u_ssao"), 0);
    }

  public:
    void BindSSAOTexture(GLuint texture) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);
    }
  };
}

#endif
