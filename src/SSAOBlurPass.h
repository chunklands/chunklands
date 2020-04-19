#ifndef __CHUNKLANDS_SSAOBLURPASS_H__
#define __CHUNKLANDS_SSAOBLURPASS_H__

#include "RenderPass.h"

namespace chunklands {
  class SSAOBlurPass : public RenderPass {
  protected:
    void InitializeProgram();

  public:
    void BindSSAOTexture(GLuint texture);
  };
}

#endif
