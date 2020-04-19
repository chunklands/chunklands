#ifndef __CHUNKLANDS_SSAOPASS_H__
#define __CHUNKLANDS_SSAOPASS_H__

#include <glm/mat4x4.hpp>

#include "RenderPass.h"

namespace chunklands {
  class SSAOPass : public RenderPass {
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
