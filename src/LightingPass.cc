#include "LightingPass.h"

#include <glm/gtc/type_ptr.hpp>

namespace chunklands {
  void LightingPass::InitializeProgram() {
    uniforms_ = {
      .render_distance = program_->GetUniformLocation("u_render_distance"),
      .sun_position     = program_->GetUniformLocation("u_sun_position"),
    };

    glUniform1i(program_->GetUniformLocation("u_position_texture"), 0);
    glUniform1i(program_->GetUniformLocation("u_normal_texture"),   1);
    glUniform1i(program_->GetUniformLocation("u_color_texture"),    2);
    glUniform1i(program_->GetUniformLocation("u_ssao_texture"),     3);
  }

  void LightingPass::UpdateRenderDistance(GLfloat value) {
    glUniform1f(uniforms_.render_distance, value);
  }

  void LightingPass::UpdateSunPosition(const glm::vec3& value) {
    glUniform3fv(uniforms_.sun_position, 1, glm::value_ptr(value));
  }

  void LightingPass::BindPositionTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  void LightingPass::BindNormalTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  void LightingPass::BindColorTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  void LightingPass::BindSSAOTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture);
  }
}
