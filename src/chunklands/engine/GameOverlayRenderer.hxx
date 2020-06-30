#ifndef __CHUNKLANDS_ENGINE_GAMEOVERLAYRENDERER_HXX__
#define __CHUNKLANDS_ENGINE_GAMEOVERLAYRENDERER_HXX__

#include <chunklands/js.hxx>
#include "RenderPass.hxx"
#include <chunklands/modules/gl/glfw.hxx>

namespace chunklands::engine {

  class GameOverlayRenderer : public JSObjectWrap<GameOverlayRenderer>, public RenderPass {
    JS_IMPL_WRAP(GameOverlayRenderer, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    virtual void Begin() override {
      glDepthFunc(GL_ALWAYS);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      RenderPass::Begin();
    }

    virtual void End() override {
      RenderPass::End();
      glDepthFunc(GL_LESS);
      glDisable(GL_BLEND);
    }

    void UpdateProjection() {
      glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(proj_));
    }

    void UpdateBufferSize(int width, int height) override {
      const float scale = 4.f; // TODO(daaitch): use window pixel scale (4k screens)
      proj_ = glm::ortho(-float(width) / scale, float(width) / scale, -float(height) / scale, float(height) / scale);
    }

  protected:
    void InitializeProgram() override {
      modules::gl::Uniform texture{"u_texture"};
      *js_Program >> texture >> uniforms_.proj;
      texture.Update(0);
    }

  private:
    struct {
      modules::gl::Uniform proj{"u_proj"};
    } uniforms_;

    glm::mat4 proj_;
  };

} // namespace chunklands::engine

#endif