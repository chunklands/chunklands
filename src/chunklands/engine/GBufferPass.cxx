
#include "GBufferPass.hxx"
#include "gl/gl_exception.hxx"
#include "gl/gl_check.hxx"

namespace chunklands::engine {

  GBufferPass::GBufferPass(Window* window, std::unique_ptr<gl::Program> program)
    : program_(std::move(program))
    , u_proj(program_->GetUniformLocation("u_proj"))
    , u_view(program_->GetUniformLocation("u_view"))
    , u_texture(program_->GetUniformLocation("u_texture"))
  {
    assert(window);
    const window_size_t size = window->GetSize();
    UpdateBuffers(size.width, size.height);
    window_resize_conn_ = window->on_resize.connect([this](const window_size_t& size) {
      UpdateBuffers(size.width, size.height);
    });
  }

  GBufferPass::~GBufferPass() {
    std::cout << "~GBufferPass" << std::endl;
    DeleteBuffers();
  }

  void GBufferPass::UpdateBuffers(int width, int height) {
    GL_CHECK_DEBUG();

    DeleteBuffers();
    GL_CHECK_DEBUG();

    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    GL_CHECK_DEBUG();

    glGenTextures(1, &texture_position);
    glBindTexture(GL_TEXTURE_2D, texture_position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_position, 0);
    GL_CHECK_DEBUG();

    glGenTextures(1, &texture_normal);
    glBindTexture(GL_TEXTURE_2D, texture_normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture_normal, 0);
    GL_CHECK_DEBUG();

    glGenTextures(1, &texture_color);
    glBindTexture(GL_TEXTURE_2D, texture_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texture_color, 0);
    GL_CHECK_DEBUG();

    const GLuint attachments[3] = {
      GL_COLOR_ATTACHMENT0,
      GL_COLOR_ATTACHMENT1,
      GL_COLOR_ATTACHMENT2
    };

    glDrawBuffers(3, attachments);
    GL_CHECK_DEBUG();

    glGenRenderbuffers(1, &renderbuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_);
    GL_CHECK_DEBUG();

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      gl::throw_gl_exception("glFramebufferRenderbuffer", "framebuffer status not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GL_CHECK_DEBUG();
  }

  void GBufferPass::DeleteBuffers() {
    GL_CHECK_DEBUG();

    glDeleteTextures(1, &texture_position);
    glDeleteTextures(1, &texture_normal);
    glDeleteTextures(1, &texture_color);
    glDeleteRenderbuffers(1, &renderbuffer_);
    glDeleteFramebuffers(1, &framebuffer_);

    GL_CHECK_DEBUG();
  }

} // namespace chunklands::engine