#ifndef __CHUNKLANDS_ENGINE_SSAOPASS_HXX__
#define __CHUNKLANDS_ENGINE_SSAOPASS_HXX__

#include <chunklands/js.hxx>
#include <chunklands/gl/Uniform.hxx>
#include "RenderPass.hxx"

namespace chunklands::engine {

  class SSAOPass : public JSObjectWrap<SSAOPass>, public RenderPass {
    JS_IMPL_WRAP(SSAOPass, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    ~SSAOPass() {
      DeleteBuffers();
    }

  public:
    void Begin() override {
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
      glClearColor(0.f, 0.f, 0.f, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
      RenderPass::Begin();
      BindNoiseTexture();
    }

    void End() override {
      RenderPass::End();
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void UpdateProjection(const glm::mat4& proj) {
      glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(proj));
    }

    void BindPositionTexture(GLuint texture) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);
    }

    void BindNormalTexture(GLuint texture) {
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture);
    }

    void UpdateBufferSize(int width, int height) override;
    void DeleteBuffers();

  protected:
    void InitializeProgram() override;

  private:
    void BindNoiseTexture() {
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, textures_.noise);
    }

  public:
    struct {
      gl::Uniform proj{"u_proj"};
    } uniforms_;

    GLuint framebuffer_ = 0;

    struct {
      GLuint color = 0;
      GLuint noise = 0;
    } textures_;
  };

} // namespace chunklands::engine

#endif