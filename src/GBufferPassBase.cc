#include "GBufferPassBase.h"

#include <glm/gtc/type_ptr.hpp>

namespace chunklands {
  JS_DEF_WRAP(GBufferPassBase, ONE_ARG({
    JS_SETTER(Program)
  }))

  void GBufferPassBase::JSCall_Set_Program(const Napi::CallbackInfo& info) {
    js_Program = info[0];

    js_Program->Use();

    uniforms_ = {
      .proj     = js_Program->GetUniformLocation("u_proj"),
      .view     = js_Program->GetUniformLocation("u_view"),
    };

    glUniform1i(js_Program->GetUniformLocation("u_texture"), 0);

    js_Program->Unuse();
  }

  void GBufferPassBase::UpdateProjection(const glm::mat4& proj) {
    glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(proj));
  }

  void GBufferPassBase::UpdateView(const glm::mat4& view) {
    glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
  }
}
