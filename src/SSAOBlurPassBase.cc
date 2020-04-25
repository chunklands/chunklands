#include "SSAOBlurPassBase.h"

namespace chunklands {
  JS_DEF_WRAP(SSAOBlurPassBase, ONE_ARG({
    JS_SETTER(Program)
  }))

  void SSAOBlurPassBase::InitializeProgram() {
    glUniform1i(js_Program->GetUniformLocation("u_ssao"), 0);
  }

  void SSAOBlurPassBase::BindSSAOTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
  }
}
