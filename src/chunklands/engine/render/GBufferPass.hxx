#ifndef __CHUNKLANDS_ENGINE_RENDER_GBUFFERPASS_HXX__
#define __CHUNKLANDS_ENGINE_RENDER_GBUFFERPASS_HXX__

#include <boost/move/core.hpp>
#include <chunklands/engine/gl/Program.hxx>
#include <chunklands/engine/gl/Uniform.hxx>
#include <memory>

namespace chunklands::engine::render {

class GBufferPass {
public:
    GBufferPass(std::unique_ptr<gl::Program> program);
    ~GBufferPass();

    void UpdateBuffers(int width, int height);

    void BeginPass(const glm::mat4& proj, const glm::mat4& view,
        const glm::vec3& camera_pos);
    void EndPass();

    void SetNewFactor(float new_factor)
    {
        u_new_factor.Update(new_factor);
    }

    void LoadTexture(GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);

    GLuint GetPositionTexture() const
    {
        return texture_position_;
    }

    GLuint GetNormalTexture() const
    {
        return texture_normal_;
    }

    GLuint GetColorTexture() const
    {
        return texture_color_;
    }

    GLuint GetBakeTexture() const
    {
        return texture_;
    }

private:
    void DeleteBuffers();

private:
    std::unique_ptr<gl::Program> program_;
    gl::Uniform<glm::mat4> u_proj_;
    gl::Uniform<glm::mat4> u_view_;

    gl::Uniform<GLint> u_texture_;
    gl::Uniform<GLfloat> u_new_factor;
    gl::Uniform<glm::vec3> u_camera_pos;

    GLuint renderbuffer_ = 0;
    GLuint framebuffer_ = 0;

    GLuint texture_position_ = 0;
    GLuint texture_normal_ = 0;
    GLuint texture_color_ = 0;

    GLuint texture_ = 0;

    BOOST_MOVABLE_BUT_NOT_COPYABLE(GBufferPass)
};

} // namespace chunklands::engine::render

#endif