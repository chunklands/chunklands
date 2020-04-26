#include "GBufferPassBase.h"

namespace chunklands {
  JS_DEF_WRAP(GBufferPassBase)

  void GBufferPassBase::InitializeProgram() {
    uniforms_ = {
      .proj     = js_Program->GetUniformLocation("u_proj"),
      .view     = js_Program->GetUniformLocation("u_view"),
    };

    glUniform1i(js_Program->GetUniformLocation("u_texture"), 0);
  }
}
