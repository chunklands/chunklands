
#include "GameOverlayRenderer.hxx"

namespace chunklands::engine {

  JS_DEF_WRAP(GameOverlayRenderer)

  void GameOverlayRenderer::Begin() {
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    RenderPass::Begin();
  }

  void GameOverlayRenderer::End() {
    RenderPass::End();
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
  }

  void GameOverlayRenderer::UpdateProjection() {
    glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(proj_));
  }

  void GameOverlayRenderer::UpdateBufferSize(int, int) {
    proj_ = glm::ortho(0.f, 1.f, 0.f, 1.f);
  }

  void GameOverlayRenderer::InitializeProgram() {
    gl::Uniform texture{"u_texture"};
    *js_Program >> texture >> uniforms_.proj;
    texture.Update(0);
  }

} // namespace chunklands::engine