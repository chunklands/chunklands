
#include "BlockSelectPass.hxx"

namespace chunklands::engine {

  JS_DEF_WRAP(BlockSelectPass)

  void BlockSelectPass::Begin() {
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    RenderPass::Begin();
  }

  void BlockSelectPass::End() {
    RenderPass::End();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
  }

  void BlockSelectPass::UpdateProjection(const glm::mat4& matrix) {
    glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void BlockSelectPass::UpdateView(const glm::mat4& matrix) {
    glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void BlockSelectPass::UpdateBlock(const std::unordered_map<std::string, std::vector<GLfloat>>& faces) {
    CHECK_GL();
    std::vector<GLfloat> vertices;
    for (auto&& x : faces) {
      vertices.insert(vertices.cend(), x.second.cbegin(), x.second.cend());
    }

    CHECK_GL_HERE();
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    CHECK_GL_HERE();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    CHECK_GL_HERE();

    vertex_count_ = vertices.size() / 8;
  }

  void BlockSelectPass::Render() {
    CHECK_GL();
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count_);
    glBindVertexArray(0);
    CHECK_GL_HERE();
  }

  void BlockSelectPass::InitializeProgram() {
    CHECK_GL();
    *js_Program >> uniforms_.proj >> uniforms_.view;

    CHECK_GL_HERE();
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    CHECK_GL_HERE();
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    CHECK_GL_HERE();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)0);

    CHECK_GL_HERE();
    glBindVertexArray(0);
  }

} // namespace chunklands::engine