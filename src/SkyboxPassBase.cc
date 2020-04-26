#include "SkyboxPassBase.h"

namespace chunklands {
  JS_DEF_WRAP(SkyboxPassBase)
  
  void SkyboxPassBase::JSCall_useProgram(JSCbi info) {
    CHECK_GL();
    program_ = info[0];
    program_->Use();

    uniforms_ = {
      .proj = program_->GetUniformLocation("u_proj"),
      .view = program_->GetUniformLocation("u_view")
    };

    glUniform1i(program_->GetUniformLocation("u_skybox_texture"), 0);
    program_->Unuse();
  }
}
