#include "SkyboxPassBase.h"

#include <glm/gtc/type_ptr.hpp>

namespace chunklands {
  JS_DEF_WRAP(SkyboxPassBase, ONE_ARG({
    JS_CB(useProgram)
  }))
  
  Napi::Value SkyboxPassBase::JSCall_useProgram(const Napi::CallbackInfo& info) {
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

  void SkyboxPassBase::UpdateProjection(const glm::mat4& matrix) {
    CHECK_GL();
    glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void SkyboxPassBase::UpdateView(const glm::mat4& matrix) {
    CHECK_GL();
    glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void SkyboxPassBase::BindSkyboxTexture(GLuint texture) {
    CHECK_GL();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
  }
}
