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
    void Begin() override;
    void End() override;

    void UpdateProjection(const glm::mat4& matrix);
    void UpdateView(const glm::mat4& matrix);
    void UpdateBlock(const std::unordered_map<std::string, std::vector<GLfloat>>& faces);
    void Render();

  protected:
    void InitializeProgram() override;
    
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