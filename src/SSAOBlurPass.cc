#include "SSAOBlurPass.h"

namespace chunklands {
  void SSAOBlurPass::InitializeProgram() {
    glUniform1i(program_->GetUniformLocation("u_ssao"), 0);
  }

  void SSAOBlurPass::BindSSAOTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
  }
}
