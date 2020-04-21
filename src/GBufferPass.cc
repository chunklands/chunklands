#include "GBufferPass.h"

#include <glm/gtc/type_ptr.hpp>

namespace chunklands {
  void GBufferPass::InitializeProgram() {
    uniforms_ = {
      .proj     = program_->GetUniformLocation("u_proj"),
      .view     = program_->GetUniformLocation("u_view"),
    };

    glUniform1i(program_->GetUniformLocation("u_texture"), 0);
  }

  void GBufferPass::UpdateProjection(const glm::mat4& proj) {
    glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(proj));
  }

  void GBufferPass::UpdateView(const glm::mat4& view) {
    glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
  }
}
