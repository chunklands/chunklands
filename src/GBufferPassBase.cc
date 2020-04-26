#include "GBufferPassBase.h"

namespace chunklands {
  JS_DEF_WRAP(GBufferPassBase)

  void GBufferPassBase::JSCall_SetProgram(JSCbi info) {
    js_Program = info[0];

    js_Program->Use();

    uniforms_ = {
      .proj     = js_Program->GetUniformLocation("u_proj"),
      .view     = js_Program->GetUniformLocation("u_view"),
    };

    glUniform1i(js_Program->GetUniformLocation("u_texture"), 0);

    js_Program->Unuse();
  }
}
