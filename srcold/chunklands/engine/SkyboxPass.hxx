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
    void Begin() override;
    void End() override;

    void UpdateProjection(const glm::mat4& matrix);
    void UpdateView(const glm::mat4& matrix);
    void BindSkyboxTexture(GLuint texture);

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