#include "SkyboxPassBase.h"

namespace chunklands {
  JS_DEF_WRAP(SkyboxPassBase)
  
  void SkyboxPassBase::InitializeProgram() {
    CHECK_GL();

    uniforms_ = {
      .proj = js_Program->GetUniformLocation("u_proj"),
      .view = js_Program->GetUniformLocation("u_view")
    };

    glUniform1i(js_Program->GetUniformLocation("u_skybox_texture"), 0);
  }
}
