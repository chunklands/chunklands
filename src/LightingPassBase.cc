#include "LightingPassBase.h"

#include <glm/gtc/type_ptr.hpp>

namespace chunklands {
  JS_DEF_WRAP(LightingPassBase, ONE_ARG({
    JS_SETTER(Program)
  }))

  void LightingPassBase::InitializeProgram() {
    uniforms_ = {
      .render_distance = js_Program->GetUniformLocation("u_render_distance"),
      .sun_position     = js_Program->GetUniformLocation("u_sun_position"),
    };

    glUniform1i(js_Program->GetUniformLocation("u_position_texture"), 0);
    glUniform1i(js_Program->GetUniformLocation("u_normal_texture"),   1);
    glUniform1i(js_Program->GetUniformLocation("u_color_texture"),    2);
    glUniform1i(js_Program->GetUniformLocation("u_ssao_texture"),     3);
  }

  void LightingPassBase::UpdateRenderDistance(GLfloat value) {
    glUniform1f(uniforms_.render_distance, value);
  }

  void LightingPassBase::UpdateSunPosition(const glm::vec3& value) {
    glUniform3fv(uniforms_.sun_position, 1, glm::value_ptr(value));
  }

  void LightingPassBase::BindPositionTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  void LightingPassBase::BindNormalTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  void LightingPassBase::BindColorTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  void LightingPassBase::BindSSAOTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture);
  }
}
