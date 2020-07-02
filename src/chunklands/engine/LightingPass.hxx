#ifndef __CHUNKLANDS_ENGINE_LIGHTINGPASS_HXX__
#define __CHUNKLANDS_ENGINE_LIGHTINGPASS_HXX__

#include <chunklands/gl/Uniform.hxx>
#include "RenderPass.hxx"
#include <chunklands/js.hxx>

namespace chunklands::engine {

  class LightingPass : public JSObjectWrap<LightingPass>, public RenderPass {
    JS_IMPL_WRAP(LightingPass, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    void Begin() override;
    void End() override;

    void UpdateRenderDistance(GLfloat value);
    void UpdateSunPosition(const glm::vec3& value);
    void BindPositionTexture(GLuint texture);
    void BindNormalTexture(GLuint texture);
    void BindColorTexture(GLuint texture);
    void BindSSAOTexture(GLuint texture);

  protected:
    void InitializeProgram() override;

  private:
    struct {
      gl::Uniform render_distance{"u_render_distance"},
                sun_position{"u_sun_position"};
    } uniforms_;
  };

} // namespace chunklands::engine

#endif