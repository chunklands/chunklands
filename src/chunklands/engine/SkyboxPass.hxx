#ifndef __CHUNKLANDS_ENGINE_SKYBOXPASS_HXX__
#define __CHUNKLANDS_ENGINE_SKYBOXPASS_HXX__

#include <chunklands/js.hxx>
#include "RenderPass.hxx"
#include <chunklands/gl/Uniform.hxx>

namespace chunklands::engine {

  class SkyboxPass : public JSObjectWrap<SkyboxPass>, public RenderPass {
    JS_IMPL_WRAP(SkyboxPass, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    void Begin() override {
      glDisable(GL_CULL_FACE);
      glDepthFunc(GL_LEQUAL);
      RenderPass::Begin();
    }

    void End() override {
      RenderPass::End();
      glDepthFunc(GL_LESS);
      glEnable(GL_CULL_FACE);
    }

    void UpdateProjection(const glm::mat4& matrix) {
      glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void UpdateView(const glm::mat4& matrix) {
      glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void BindSkyboxTexture(GLuint texture) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);
    }

  protected:
    void InitializeProgram() override;
    
  private:

    struct {
      gl::Uniform proj {"u_proj"},
                  view {"u_view"};
    } uniforms_;
  };

} // namespace chunklands::engine

#endif