#ifndef __CHUNKLANDS_ENGINE_GBUFFERPASS_HXX__
#define __CHUNKLANDS_ENGINE_GBUFFERPASS_HXX__

#include "gl/Program.hxx"
#include "gl/Uniform.hxx"
#include <memory>
#include "Window.hxx"
#include <boost/move/core.hpp>

namespace chunklands::engine {

  class GBufferPass {
  public:
    GBufferPass(Window* window, std::unique_ptr<gl::Program> program);
    ~GBufferPass();

    void DeleteBuffers();
    void UpdateBuffers(int width, int height);

    void BeginPass();
    void EndPass();

    void LoadTexture(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);

    GLuint GetPositionTexture() const {
      return texture_position_;
    }

    GLuint GetNormalTexture() const {
      return texture_normal_;
    }

    GLuint GetColorTexture() const {
      return texture_color_;
    }

    void UpdateView(const glm::vec3& eye, const glm::vec3& center);
  
  private:
    void UpdateProj(int width, int height);

  private:
    std::unique_ptr<gl::Program> program_;
    gl::Uniform<glm::mat4> u_proj_;
    glm::mat4 proj_;

    gl::Uniform<glm::mat4> u_view_;
    glm::mat4 view_;

    gl::Uniform<GLint> u_texture_;

    GLuint renderbuffer_ = 0;
    GLuint framebuffer_  = 0;

    GLuint texture_position_ = 0;
    GLuint texture_normal_ = 0;
    GLuint texture_color_ = 0;

    GLuint texture_ = 0;

    boost::signals2::scoped_connection window_resize_conn_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE(GBufferPass)
  };

} // namespace chunklands::engine

#endif