
#include "GBufferPass.hxx"
#include "gl/gl_check.hxx"
#include "gl/gl_exception.hxx"

namespace chunklands::engine {

GBufferPass::GBufferPass(Window* window, std::unique_ptr<gl::Program> program)
    : program_(std::move(program))
    , u_proj_(*program_, "u_proj")
    , u_view_(*program_, "u_view")
    , u_texture_(*program_, "u_texture")
{
    assert(window);
    const window_size_t size = window->GetSize();
    UpdateBuffers(size.width, size.height);
    UpdateProj(size.width, size.height);

    window_resize_conn_ = window->on_resize.connect([this](const window_size_t& size) {
        UpdateBuffers(size.width, size.height);
        UpdateProj(size.width, size.height);
    });

    view_ = glm::lookAt(glm::vec3(12, -4, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    program_->Use();
    u_texture_.Update(0);
    program_->Unuse();
}

GBufferPass::~GBufferPass()
{
    std::cout << "~GBufferPass" << std::endl;
    DeleteBuffers();

    if (texture_ != 0) {
        glDeleteTextures(1, &texture_);
    }
}

void GBufferPass::UpdateBuffers(int width, int height)
{
    GL_CHECK_DEBUG();

    DeleteBuffers();
    GL_CHECK_DEBUG();

    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    GL_CHECK_DEBUG();

    glGenTextures(1, &texture_position_);
    glBindTexture(GL_TEXTURE_2D, texture_position_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_position_, 0);
    GL_CHECK_DEBUG();

    glGenTextures(1, &texture_normal_);
    glBindTexture(GL_TEXTURE_2D, texture_normal_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture_normal_, 0);
    GL_CHECK_DEBUG();

    glGenTextures(1, &texture_color_);
    glBindTexture(GL_TEXTURE_2D, texture_color_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texture_color_, 0);
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

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        gl::throw_gl_exception("glFramebufferRenderbuffer", "framebuffer status not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GL_CHECK_DEBUG();
}

void GBufferPass::DeleteBuffers()
{
    GL_CHECK_DEBUG();

    glDeleteTextures(1, &texture_position_);
    glDeleteTextures(1, &texture_normal_);
    glDeleteTextures(1, &texture_color_);
    glDeleteRenderbuffers(1, &renderbuffer_);
    glDeleteFramebuffers(1, &framebuffer_);

    GL_CHECK_DEBUG();
}

void GBufferPass::BeginPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (texture_ != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_);
    }

    program_->Use();
    u_proj_.Update(proj_);
    u_view_.Update(view_);
}

void GBufferPass::EndPass()
{
    program_->Unuse();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBufferPass::UpdateProj(int width, int height)
{
    glViewport(0, 0, width, height);
    proj_ = glm::perspective(glm::radians(75.f), float(width) / float(height), 0.1f, 1000.0f);
}

void GBufferPass::LoadTexture(GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
    // void GBufferPass::LoadTexture(GLsizei, GLsizei, GLenum, GLenum, const void*) {
    GL_CHECK_DEBUG();

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, type, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    GL_CHECK_DEBUG();
    // const unsigned char p[] = {255, 255, 200, 255};
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &p);
    glBindTexture(GL_TEXTURE_2D, 0);
    GL_CHECK_DEBUG();
}

void GBufferPass::UpdateView(const glm::vec3& eye, const glm::vec3& center)
{
    view_ = glm::lookAt(eye, center, glm::vec3(0, 1, 0));
}

} // namespace chunklands::engine