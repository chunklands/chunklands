#ifndef __CHUNKLANDS_SSAOPASSBASE_H__
#define __CHUNKLANDS_SSAOPASSBASE_H__

#include <glm/mat4x4.hpp>

#include "js.h"
#include "gl.h"

#include "GLProgramBase.h"
#include "ARenderPass.h"

namespace chunklands {
  class SSAOPassBase : public JSWrap<SSAOPassBase>, public ARenderPass {
    JS_DECL_WRAP(SSAOPassBase)

  public:
    void UpdateProjection(const glm::mat4& proj);
    void BindPositionTexture(GLuint texture);
    void BindNormalTexture(GLuint texture);
    void BindNoiseTexture(GLuint texture);

  protected:
    void InitializeProgram() override;

  private:
    struct {
      GLint proj = -1;
    } uniforms_;
  };
}

#endif
