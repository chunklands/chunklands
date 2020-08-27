
#include "LightingPass.hxx"

namespace chunklands::engine::render {

LightingPass::LightingPass(std::unique_ptr<gl::Program> program)
    : program_(std::move(program))
    // , u_render_distance_(*program_, "u_render_distance")
    // , u_sun_position_(*program_, "u_sun_position")
    // , u_position_texture(*program_, "u_position_texture")
    // , u_normal_texture(*program_, "u_normal_texture")
    , u_color_texture(*program_, "u_color_texture")
{
    program_->Use();
    // u_position_texture.Update(0);
    // u_normal_texture.Update(1);
    u_color_texture.Update(2);
    program_->Unuse();
}

void LightingPass::BeginPass(GLuint /*position_texture*/, GLuint /*normal_texture*/, GLuint color_texture)
{
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program_->Use();

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, position_texture);

    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, normal_texture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, color_texture);
}

void LightingPass::EndPass()
{
    program_->Unuse();
}

} // namespace chunklands::engine::render