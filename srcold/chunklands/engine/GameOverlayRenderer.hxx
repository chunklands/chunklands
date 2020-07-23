#ifndef __CHUNKLANDS_ENGINE_GAMEOVERLAYRENDERER_HXX__
#define __CHUNKLANDS_ENGINE_GAMEOVERLAYRENDERER_HXX__

#include <chunklands/js.hxx>
#include "RenderPass.hxx"
#include <chunklands/gl/glfw.hxx>
#include <chunklands/gl/Uniform.hxx>

namespace chunklands::engine {

  class GameOverlayRenderer : public JSObjectWrap<GameOverlayRenderer>, public RenderPass {
    JS_IMPL_WRAP(GameOverlayRenderer, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    void Begin() override;
    void End() override;

    void UpdateProjection();
    void UpdateBufferSize(int width, int height) override;

  protected:
    void InitializeProgram() override;

  private:
    struct {
      gl::Uniform proj{"u_proj"};
    } uniforms_;

    glm::mat4 proj_;
  };

} // namespace chunklands::engine

#endif