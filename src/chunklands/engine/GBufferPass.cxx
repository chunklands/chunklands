
#include "GBufferPass.hxx"

namespace chunklands::engine {

  JS_DEF_WRAP(GBufferPass)

  void GBufferPass::InitializeProgram() {
    modules::gl::Uniform texture {"u_texture"};
    *js_Program >> uniforms_.proj >> uniforms_.view >> texture;

    texture.Update(0);
  }

  void GBufferPass::UpdateBufferSize(int width, int height) {
    DeleteBuffers();

    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    glGenTextures(1, &textures_.position);
    glBindTexture(GL_TEXTURE_2D, textures_.position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures_.position, 0);

    glGenTextures(1, &textures_.normal);
    glBindTexture(GL_TEXTURE_2D, textures_.normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textures_.normal, 0);

    glGenTextures(1, &textures_.color);
    glBindTexture(GL_TEXTURE_2D, textures_.color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textures_.color, 0);

    const GLuint attachments[3] = {
      GL_COLOR_ATTACHMENT0,
      GL_COLOR_ATTACHMENT1,
      GL_COLOR_ATTACHMENT2
    };

    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &renderbuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void GBufferPass::DeleteBuffers() {
    glDeleteTextures(1, &textures_.position);
    glDeleteTextures(1, &textures_.normal);
    glDeleteTextures(1, &textures_.color);
    glDeleteRenderbuffers(1, &renderbuffer_);
    glDeleteFramebuffers(1, &framebuffer_);
  }

} // namespace chunklands::engine