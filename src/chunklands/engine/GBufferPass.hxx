#ifndef __CHUNKLANDS_ENGINE_GBUFFERPASS_HXX__
#define __CHUNKLANDS_ENGINE_GBUFFERPASS_HXX__

#include <chunklands/js.hxx>
#include "RenderPass.hxx"
#include <chunklands/gl/Uniform.hxx>

namespace chunklands::engine {

  class GBufferPass : public JSObjectWrap<GBufferPass>, public RenderPass {
    JS_IMPL_WRAP(GBufferPass, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    virtual ~GBufferPass() {
      DeleteBuffers();
    }

  public:
    void Begin() override {
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
      glClearColor(0.f, 0.f, 0.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      RenderPass::Begin();
    }

    void End() override {
      RenderPass::End();
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void UpdateProjection(const glm::mat4& proj) {
      glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(proj));
    }

    void UpdateView(const glm::mat4& view) {
      glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
    }

    void UpdateBufferSize(int width, int height) override;
    void DeleteBuffers();

  protected:
    void InitializeProgram() override;

  public:
    struct {
      gl::Uniform proj {"u_proj"},
                  view {"u_view"};
    } uniforms_;

    GLuint renderbuffer_ = 0;
    GLuint framebuffer_  = 0;

    struct {
      GLuint position = 0;
      GLuint normal   = 0;
      GLuint color    = 0;
    } textures_;
  };

} // namespace chunklands::engine

#endif