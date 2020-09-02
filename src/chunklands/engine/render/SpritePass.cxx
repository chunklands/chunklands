
#include "SpritePass.hxx"

namespace chunklands::engine::render {

SpritePass::SpritePass(std::unique_ptr<gl::Program> program)
    : program_(std::move(program))
    , u_proj_(*program_, "u_proj")
    , u_texture_(*program_, "u_texture")
{
    program_->Use();
    u_texture_.Update(0);
    program_->Unuse();
}

void SpritePass::BeginPass(const glm::mat4& proj, GLuint texture)
{
    glDepthFunc(GL_ALWAYS);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (texture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    program_->Use();
    u_proj_.Update(proj);
}

void SpritePass::EndPass()
{
    program_->Unuse();
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

} // namespace chunklands::engine::render