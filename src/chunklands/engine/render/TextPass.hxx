#ifndef __CHUNKLANDS_ENGINE_RENDER_TEXTPASS_HXX__
#define __CHUNKLANDS_ENGINE_RENDER_TEXTPASS_HXX__

#include <chunklands/engine/gl/Program.hxx>
#include <chunklands/engine/gl/Uniform.hxx>
#include <glm/mat4x4.hpp>
#include <memory>

namespace chunklands::engine::render {

class TextPass {
public:
    TextPass(std::unique_ptr<gl::Program> program);

    void BeginPass(const glm::mat4& proj);
    void EndPass();

    void SetPosition(const glm::vec2& position)
    {
        u_pos_.Update(position);
    }

    void SetTexture(GLuint texture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void UpdateBufferSize(int width, int height);
    void Render();

private:
    std::unique_ptr<gl::Program> program_;
    gl::Uniform<GLint> u_texture_;
    gl::Uniform<glm::mat4> u_proj_;
    gl::Uniform<glm::vec2> u_pos_;
};

} // namespace chunklands::engine::render

#endif