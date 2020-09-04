
#include "TextPass.hxx"

namespace chunklands::engine::render {

TextPass::TextPass(std::unique_ptr<gl::Program> program)
    : program_(std::move(program))
    , u_texture_(*program_, "u_texture")
    , u_proj_(*program_, "u_proj")
    , u_pos_(*program_, "u_pos")
{
    program_->Use();
    u_texture_.Update(0);
    program_->Unuse();
}

void TextPass::BeginPass(const glm::mat4& proj)
{
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program_->Use();

    u_proj_.Update(proj);
}

void TextPass::EndPass()
{
    program_->Unuse();
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
}

} // namespace chunklands::engine::render