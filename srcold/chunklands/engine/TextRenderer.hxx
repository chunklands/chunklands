#ifndef __CHUNKLANDS_ENGINE_TEXTRENDERER_HXX__
#define __CHUNKLANDS_ENGINE_TEXTRENDERER_HXX__

#include <chunklands/js.hxx>
#include "RenderPass.hxx"
#include "FontLoader.hxx"
#include <chunklands/gl/Uniform.hxx>

namespace chunklands::engine {

  class TextRenderer : public JSObjectWrap<TextRenderer>, public RenderPass {
    JS_IMPL_WRAP(TextRenderer, ONE_ARG({
      JS_SETTER(Program),
      JS_SETTER(FontLoader),
      JS_CB(write),
    }))

    JS_IMPL_SETTER_WRAP(FontLoader, FontLoader)
    JS_DECL_CB_VOID(write)

  public:
    void Begin() override;
    void End() override;

    void UpdateBufferSize(int width, int height) override;
    void Render();

  protected:
    void InitializeProgram() override;

  private:
    GLuint  vao_ = 0,
            vbo_ = 0;
    GLsizei count_ = 0;

    glm::mat4 proj_;

    struct {
      gl::Uniform proj{"u_proj"};
    } uniforms_;

    int height_ = 0;
  };

} // namespace chunklands::engine

#endif