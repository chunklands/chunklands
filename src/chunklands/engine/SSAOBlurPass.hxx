#ifndef __CHUNKLANDS_ENGINE_SSAOBLURPASS_HXX__
#define __CHUNKLANDS_ENGINE_SSAOBLURPASS_HXX__

#include <chunklands/js.hxx>
#include "RenderPass.hxx"
#include <chunklands/gl/Uniform.hxx>

namespace chunklands::engine {

  class SSAOBlurPass : public JSObjectWrap<SSAOBlurPass>, public RenderPass {
    JS_IMPL_WRAP(SSAOBlurPass, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    void Begin() override;
    void End() override;

    void BindSSAOTexture(GLuint texture);
    void UpdateBufferSize(int width, int height) override;
    void DeleteBuffers();

  protected:
    void InitializeProgram() override;

  public:
    GLuint framebuffer_ = 0;

    struct {
      GLuint color = 0;
    } textures_;
  };

} // namespace chunklands::engine

#endif