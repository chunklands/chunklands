#include "SSAOBlurPassBase.h"

namespace chunklands {
  JS_DEF_WRAP(SSAOBlurPassBase)

  void SSAOBlurPassBase::UpdateBufferSize(int width, int height) {
    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glGenTextures(1, &textures_.color);
    glBindTexture(GL_TEXTURE_2D, textures_.color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures_.color, 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void SSAOBlurPassBase::DeleteBuffers() {
    glDeleteTextures(1, &textures_.color);
    glDeleteFramebuffers(1, &framebuffer_);
  }
}
