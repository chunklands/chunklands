#ifndef __CHUNKLANDS_SSAOPASSBASE_H__
#define __CHUNKLANDS_SSAOPASSBASE_H__

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "js.h"
#include "gl.h"

#include "GLProgramBase.h"
#include "RenderPass.h"
#include "GLUniform.h"

namespace chunklands {
  class SSAOPassBase : public JSObjectWrap<SSAOPassBase>, public RenderPass {
    JS_IMPL_WRAP(SSAOPassBase, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    void Begin() {
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
      glClearColor(0.f, 0.f, 0.f, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
      RenderPass::Begin();
      BindNoiseTexture();
    }

    void End() {
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
      GLUniform proj{"u_proj"};
    } uniforms_;

    GLuint framebuffer_ = 0;

    struct {
      GLuint color = 0;
      GLuint noise = 0;
    } textures_;
  };
}

#endif
