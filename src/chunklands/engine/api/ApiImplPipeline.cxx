
#include "api-shared.hxx"
#include "../Window.hxx"
#include "../gl/Program.hxx"
#include "../GBufferPass.hxx"
#include "../LightingPass.hxx"
#include "../RenderQuad.hxx"

namespace chunklands::engine {

  boost::future<void>
  Api::RenderPipelineInit(CEWindowHandle* handle, CERenderPipelineInit init) {
    EASY_FUNCTION();
    API_FN();
    Window* const window = reinterpret_cast<Window*>(handle);
    CHECK(has_handle(api_data(data_)->windows, window));
    CHECK(g_buffer_pass_handle_ == nullptr);

    return EnqueueTask(executor_, [this,
      window,
      init = std::move(init)
    ]() {
      EASY_FUNCTION();
      
      assert(window);

      {
        std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(init.gbuffer.vertex_shader.data(), init.gbuffer.fragment_shader.data());
        std::unique_ptr<GBufferPass> g_buffer_pass = std::make_unique<GBufferPass>(window, std::move(program));
        g_buffer_pass_handle_ = reinterpret_cast<CEHandle*>(g_buffer_pass.get());
        g_buffer_pass.release();
      }
      {
        std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(init.lighting.vertex_shader.data(), init.lighting.fragment_shader.data());
        std::unique_ptr<LightingPass> lighting_pass = std::make_unique<LightingPass>(std::move(program));
        lighting_pass_handle_ = reinterpret_cast<CEHandle*>(lighting_pass.get());
        lighting_pass.release();
      }
      {
        std::unique_ptr<RenderQuad> render_quad = std::make_unique<RenderQuad>();
        render_quad_handle_ = reinterpret_cast<CEHandle*>(render_quad.get());
        render_quad.release();
      }

      // depth test
      glEnable(GL_DEPTH_TEST);

      // culling
      glDisable(GL_CULL_FACE);
      // glEnable(GL_CULL_FACE);
      // glCullFace(GL_FRONT);
      // glFrontFace(GL_CCW);
    });
  }

} // namespace chunklands::engine