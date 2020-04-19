#ifndef __CHUNKLANDS_GBUFFERPASS_H__
#define __CHUNKLANDS_GBUFFERPASS_H__

#include <glm/mat4x4.hpp>

#include "RenderPass.h"

namespace chunklands {

  class GBufferPass : public RenderPass {
  public:
    void UpdateProjection(const glm::mat4& proj);
    void UpdateView(const glm::mat4& view);

  protected:
    void InitializeProgram() override;

  private:
    struct {
      GLint proj    = -1,
            view    = -1,
            texture = -1;
    } uniforms_;
  };
}

#endif
