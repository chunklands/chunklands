#ifndef __CHUNKLANDS_ENGINE_RENDER_SPRITEPASS_HXX__
#define __CHUNKLANDS_ENGINE_RENDER_SPRITEPASS_HXX__

#include <boost/move/core.hpp>
#include <chunklands/engine/gl/Program.hxx>
#include <chunklands/engine/gl/Uniform.hxx>
#include <memory>

namespace chunklands::engine::render {

class SpritePass {
public:
    SpritePass(std::unique_ptr<gl::Program> program);

    void UpdateBuffers(int width, int height);

    void BeginPass(const glm::mat4& proj, GLuint texture);
    void EndPass();

private:
    std::unique_ptr<gl::Program> program_;
    gl::Uniform<glm::mat4> u_proj_;
    gl::Uniform<GLint> u_texture_;

    BOOST_MOVABLE_BUT_NOT_COPYABLE(SpritePass)
};

} // namespace chunklands::engine::render

#endif