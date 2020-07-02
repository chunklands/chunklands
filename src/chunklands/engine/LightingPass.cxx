
#include "LightingPass.hxx"

namespace chunklands::engine {

  JS_DEF_WRAP(LightingPass)

  void LightingPass::Begin() {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderPass::Begin();
  }

  void LightingPass::End() {
    RenderPass::End();
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

  void LightingPass::InitializeProgram() {
    gl::Uniform position_texture{"u_position_texture"},
              normal_texture{"u_normal_texture"},
              color_texture{"u_color_texture"},
              ssao_texture{"u_ssao_texture"};

    *js_Program
      >> uniforms_.render_distance
      >> uniforms_.sun_position
      >> position_texture
      >> normal_texture
      >> color_texture
      >> ssao_texture;

    position_texture.Update(0);
    normal_texture.Update(1);
    color_texture.Update(2);
    ssao_texture.Update(3);
  }

} // namespace chunklands::engine