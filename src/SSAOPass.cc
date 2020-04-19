#include "SSAOPass.h"

#include <random>

#define  GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace chunklands {
  void SSAOPass::InitializeProgram() {
    uniforms_ = {
      .proj     = program_->GetUniformLocation("u_proj"),
    };
    
    glUniform1i(program_->GetUniformLocation("u_position"), 0);
    glUniform1i(program_->GetUniformLocation("u_normal"),   1);
    glUniform1i(program_->GetUniformLocation("u_noise"),    2);

    std::uniform_real_distribution<GLfloat> random_floats(0.f, 1.f);
    std::default_random_engine generator;
    for (int i = 0; i < 64; i++) {
      glm::vec3 sample(
        random_floats(generator) * 2.f - 1.f,
        random_floats(generator) * 2.f - 1.f,
        random_floats(generator)
      );

      sample = glm::normalize(sample);
      sample *= random_floats(generator);
      float scale = float(i) / 64.f;

      scale = glm::lerp(.1f, 1.f, scale * scale);
      sample *= scale;

      std::string uniform = std::string("u_samples[") + std::to_string(i) + "]";
      GLint location = program_->GetUniformLocation(uniform.c_str());
      glUniform3fv(location, 1, glm::value_ptr(sample));
    }
  }

  void SSAOPass::UpdateProjection(const glm::mat4& proj) {
    glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(proj));
  }

  void SSAOPass::BindPositionTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  void SSAOPass::BindNormalTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  void SSAOPass::BindNoiseTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);
  }
}
