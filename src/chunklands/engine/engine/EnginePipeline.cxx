
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/engine/engine/shared.hxx>
#include <chunklands/engine/GBufferPass.hxx>
#include <chunklands/engine/gl/Program.hxx>
#include <chunklands/engine/gl/RenderQuad.hxx>
#include <chunklands/engine/LightingPass.hxx>
#include <chunklands/engine/Window.hxx>

namespace chunklands::engine {

  boost::future<void>
  Engine::RenderPipelineInit(CEWindowHandle* handle, CERenderPipelineInit init) {
    EASY_FUNCTION();
    ENGINE_FN();
    Window* const window = reinterpret_cast<Window*>(handle);
    CHECK_OR_FATAL(has_handle(data_->window.windows, window));
    CHECK_OR_FATAL(data_->render_pipeline.gbuffer == nullptr);

    return EnqueueTask(data_->executors.opengl, [this,
      window,
      init = std::move(init)
    ]() {
      EASY_FUNCTION();
      
      assert(window);

      {
        std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(init.gbuffer.vertex_shader.data(),
                                                                             init.gbuffer.fragment_shader.data());
        std::unique_ptr<GBufferPass> g_buffer_pass = std::make_unique<GBufferPass>(window, std::move(program));
        data_->render_pipeline.gbuffer = g_buffer_pass.release();
      }

      {
        std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(init.lighting.vertex_shader.data(),
                                                                             init.lighting.fragment_shader.data());
        std::unique_ptr<LightingPass> lighting_pass = std::make_unique<LightingPass>(std::move(program));
        data_->render_pipeline.lighting = lighting_pass.release();
      }
      {
        std::unique_ptr<gl::RenderQuad> render_quad = std::make_unique<gl::RenderQuad>();
        data_->render_pipeline.render_quad = render_quad.release();
      }

      // depth test
      glEnable(GL_DEPTH_TEST);

      // culling
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
      glFrontFace(GL_CCW);
    });
  }

} // namespace chunklands::engine