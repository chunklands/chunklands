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
    ~SSAOPass();

  public:
    void Begin() override;
    void End() override;

    void UpdateProjection(const glm::mat4& proj);
    void BindPositionTexture(GLuint texture);
    void BindNormalTexture(GLuint texture);

    void UpdateBufferSize(int width, int height) override;
    void DeleteBuffers();

  protected:
    void InitializeProgram() override;

  private:
    void BindNoiseTexture();

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