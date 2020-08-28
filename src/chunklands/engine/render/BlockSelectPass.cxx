
#include "BlockSelectPass.hxx"

namespace chunklands::engine::render {

// clang-format off

//  VERTEX1         VERTEX2          VERTEX3

const CEVaoElementBlockSelectBlock block_vertices[] = {
    { 0.f, 1.f, 0.f },  { 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f },
    { 0.f, 1.f, 0.f },  { 1.f, 0.f, 0.f }, { 1.f, 1.f, 0.f },
    { 1.f, 1.f, 1.f },  { 1.f, 0.f, 1.f }, { 0.f, 0.f, 1.f },
    { 1.f, 1.f, 1.f },  { 0.f, 0.f, 1.f }, { 0.f, 1.f, 1.f },
    { 0.f, 1.f, 1.f },  { 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f },
    { 0.f, 1.f, 1.f },  { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f },
    { 1.f, 1.f, 0.f },  { 1.f, 0.f, 0.f }, { 1.f, 0.f, 1.f },
    { 1.f, 1.f, 0.f },  { 1.f, 0.f, 1.f }, { 1.f, 1.f, 1.f },
    { 0.f, 1.f, 1.f },  { 0.f, 1.f, 0.f }, { 1.f, 1.f, 0.f },
    { 0.f, 1.f, 1.f },  { 1.f, 1.f, 0.f }, { 1.f, 1.f, 1.f },
    { 0.f, 0.f, 0.f },  { 0.f, 0.f, 1.f }, { 1.f, 0.f, 1.f },
    { 0.f, 0.f, 0.f },  { 1.f, 0.f, 1.f }, { 1.f, 0.f, 0.f },
};

// clang-format on

BlockSelectPass::BlockSelectPass(std::unique_ptr<gl::Program> program)
    : program_(std::move(program))
    , u_proj_(*program_, "u_proj")
    , u_view_(*program_, "u_view")
{
    vao.Initialize(block_vertices, sizeof(block_vertices) / sizeof(CEVaoElementBlockSelectBlock));
}

void BlockSelectPass::MakePass(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& pos)
{
    glm::mat4 v = view * glm::translate(glm::identity<glm::mat4>(), pos);

    GL_CHECK_DEBUG();

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    program_->Use();
    u_proj_.Update(proj);
    u_view_.Update(v);

    vao.Render();

    program_->Unuse();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    GL_CHECK_DEBUG();
}

} // namespace chunklands::engine::render