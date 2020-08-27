
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"
#include <chunklands/engine/gl/Program.hxx>
#include <chunklands/engine/gl/RenderQuad.hxx>
#include <chunklands/engine/render/GBufferPass.hxx>
#include <chunklands/engine/render/LightingPass.hxx>
#include <chunklands/engine/window/Window.hxx>
#include <chunklands/libcxx/easy_profiler.hxx>

namespace chunklands::engine {

AsyncEngineResult<CENone>
Engine::RenderPipelineInit(CEWindowHandle* handle, CERenderPipelineInit init)
{
    EASY_FUNCTION();
    ENGINE_FN();

    ENGINE_CHECK(data_->render_pipeline.gbuffer == nullptr);

    return EnqueueTask(data_->executors.opengl, [this, handle, init = std::move(init)]() -> EngineResultX<CENone> {
        EASY_FUNCTION();

        window::Window* window = nullptr;
        ENGINE_CHECK(get_handle(&window, data_->window.windows, handle));

        {
            std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(init.gbuffer.vertex_shader.data(),
                init.gbuffer.fragment_shader.data());
            std::unique_ptr<render::GBufferPass> g_buffer_pass = std::make_unique<render::GBufferPass>(window, std::move(program));
            data_->render_pipeline.gbuffer = g_buffer_pass.release();
        }

        {
            std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(init.lighting.vertex_shader.data(),
                init.lighting.fragment_shader.data());
            std::unique_ptr<render::LightingPass> lighting_pass = std::make_unique<render::LightingPass>(std::move(program));
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

        assert(data_->render_pipeline.gbuffer != nullptr);
        assert(data_->render_pipeline.lighting != nullptr);
        assert(data_->render_pipeline.render_quad != nullptr);

        return Ok();
    });
}

} // namespace chunklands::engine