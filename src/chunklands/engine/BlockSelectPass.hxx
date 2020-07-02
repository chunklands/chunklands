#ifndef __CHUNKLANDS_ENGINE_BLOCKSELECTPASS_HXX__
#define __CHUNKLANDS_ENGINE_BLOCKSELECTPASS_HXX__

#include <chunklands/js.hxx>
#include "RenderPass.hxx"
#include <chunklands/gl/Uniform.hxx>
#include <unordered_map>

namespace chunklands::engine {

  class BlockSelectPass : public JSObjectWrap<BlockSelectPass>, public RenderPass {
    JS_IMPL_WRAP(BlockSelectPass, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    void Begin() override {
      glDisable(GL_CULL_FACE);
      // glDepthFunc(GL_LEQUAL);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDisable(GL_DEPTH_TEST);
      RenderPass::Begin();
    }

    void End() override {
      RenderPass::End();
      glEnable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);
      // glDepthFunc(GL_LESS);
      glEnable(GL_CULL_FACE);
    }

    void UpdateProjection(const glm::mat4& matrix) {
      glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void UpdateView(const glm::mat4& matrix) {
      glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void UpdateBlock(const std::unordered_map<std::string, std::vector<GLfloat>>& faces) {
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

    void Render() {
      CHECK_GL();
      glBindVertexArray(vao_);
      CHECK_GL_HERE();
      glDrawArrays(GL_TRIANGLES, 0, vertex_count_);
      CHECK_GL_HERE();
      glBindVertexArray(0);
      CHECK_GL_HERE();
    }

  protected:
    void InitializeProgram() override {
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
    
  private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLsizei vertex_count_ = 0;

    struct {
      gl::Uniform proj {"u_proj"},
                  view {"u_view"};
    } uniforms_;
  };

} // namespace chunklands::engine

#endif