
#include "SkyboxPass.hxx"

namespace chunklands::engine {

  JS_DEF_WRAP(SkyboxPass)

  void SkyboxPass::Begin() {
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    RenderPass::Begin();
  }

  void SkyboxPass::End() {
    RenderPass::End();
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
  }

  void SkyboxPass::UpdateProjection(const glm::mat4& matrix) {
    glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(matrix));
  }
  
  void SkyboxPass::UpdateView(const glm::mat4& matrix) {
    glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void SkyboxPass::BindSkyboxTexture(GLuint texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
  }
  
  void SkyboxPass::InitializeProgram() {
    CHECK_GL();

    gl::Uniform skybox_texture{"u_skybox_texture"};

    *js_Program
      >> skybox_texture
      >> uniforms_.proj
      >> uniforms_.view;

    skybox_texture.Update(0);
  }

} // namespace chunklands::engine