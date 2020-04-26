#ifndef __CHUNKLANDS_SSAOBLURPASSBASE_H__
#define __CHUNKLANDS_SSAOBLURPASSBASE_H__

#include "js.h"
#include "RenderPass.h"
#include "GLUniform.h"

namespace chunklands {
  class SSAOBlurPassBase : public JSObjectWrap<SSAOBlurPassBase>, public RenderPass {
    JS_IMPL_WRAP(SSAOBlurPassBase, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    void Begin() override {
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
      glClear(GL_COLOR_BUFFER_BIT);
      RenderPass::Begin();
    }

    void End() override {
      RenderPass::End();
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void BindSSAOTexture(GLuint texture) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);
    }

    void UpdateBufferSize(int width, int height) override;
    void DeleteBuffers();

  protected:
    void InitializeProgram() override {
      GLUniform ssao{"u_ssao"};
      *js_Program >> ssao;
      ssao.Update(0);
    }

  public:
    GLuint framebuffer_ = 0;

    struct {
      GLuint color = 0;
    } textures_;
  };
}

#endif
