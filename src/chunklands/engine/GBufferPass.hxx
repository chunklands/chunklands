#ifndef __CHUNKLANDS_ENGINE_GBUFFERPASS_HXX__
#define __CHUNKLANDS_ENGINE_GBUFFERPASS_HXX__

#include "gl/Program.hxx"
#include "gl/Uniform.hxx"
#include <memory>
#include "Window.hxx"

namespace chunklands::engine {

  class GBufferPass {
  public:
    GBufferPass(Window* window, std::unique_ptr<gl::Program> program);
    GBufferPass(const GBufferPass&) = delete;
    GBufferPass& operator=(const GBufferPass&) = delete;
    ~GBufferPass();

    void DeleteBuffers();
    void UpdateBuffers(int width, int height);

  private:
    std::unique_ptr<gl::Program> program_;
    gl::Uniform<glm::mat4> u_proj;
    gl::Uniform<glm::mat4> u_view;
    gl::Uniform<GLuint> u_texture;

    GLuint renderbuffer_ = 0;
    GLuint framebuffer_  = 0;

    GLuint texture_position = 0;
    GLuint texture_normal = 0;
    GLuint texture_color = 0;

    boost::signals2::scoped_connection window_resize_conn_;
  };

} // namespace chunklands::engine

#endif