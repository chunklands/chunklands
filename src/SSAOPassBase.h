#ifndef __CHUNKLANDS_SSAOPASSBASE_H__
#define __CHUNKLANDS_SSAOPASSBASE_H__

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "js.h"
#include "gl.h"

#include "GLProgramBase.h"
#include "ARenderPass.h"

namespace chunklands {
  class SSAOPassBase : public JSObjectWrap<SSAOPassBase>, public ARenderPass {
    JS_IMPL_WRAP(SSAOPassBase, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
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

    void BindNoiseTexture(GLuint texture) {
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, texture);
    }

  protected:
    void InitializeProgram() override;

  private:
    struct {
      GLint proj = -1;
    } uniforms_;
  };
}

#endif
