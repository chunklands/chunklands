
#include "Engine.hxx"
#include "EngineData.hxx"
#include "api_util.hxx"
#include <chunklands/engine/gl/Program.hxx>
#include <chunklands/engine/gl/RenderQuad.hxx>
#include <chunklands/engine/render/GBufferPass.hxx>
#include <chunklands/engine/render/LightingPass.hxx>
#include <chunklands/engine/render/SpritePass.hxx>
#include <chunklands/engine/window/Window.hxx>
#include <chunklands/libcxx/easy_profiler.hxx>

namespace chunklands::engine {

AsyncEngineResult<CENone>
Engine::RenderPipelineInit(CEWindowHandle* handle, CERenderPipelineInit init)
{
    EASY_FUNCTION();
    ENGINE_FN();

    ENGINE_CHECK(data_->render.gbuffer == nullptr);

    return EnqueueTask(data_->executors.opengl, [this, handle, init = std::move(init)]() -> EngineResultX<CENone> {
        EASY_FUNCTION();

        window::Window* window = nullptr;
        ENGINE_CHECK(get_handle(&window, data_->window.windows, handle));

        {
            std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(init.gbuffer.vertex_shader.data(),
                init.gbuffer.fragment_shader.data());
            std::unique_ptr<render::GBufferPass> g_buffer_pass = std::make_unique<render::GBufferPass>(std::move(program));
            data_->render.gbuffer = g_buffer_pass.release();
        }

        {
            std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(init.lighting.vertex_shader.data(),
                init.lighting.fragment_shader.data());
            std::unique_ptr<render::LightingPass> lighting_pass = std::make_unique<render::LightingPass>(std::move(program));
            data_->render.lighting = lighting_pass.release();
        }

        {
            std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(init.select_block.vertex_shader.data(),
                init.select_block.fragment_shader.data());
            std::unique_ptr<render::BlockSelectPass> block_select_pass = std::make_unique<render::BlockSelectPass>(std::move(program));
            data_->render.block_select = block_select_pass.release();
        }

        {
            std::unique_ptr<gl::Program> program = std::make_unique<gl::Program>(init.sprite.vertex_shader.data(),
                init.sprite.fragment_shader.data());
            std::unique_ptr<render::SpritePass> sprite_pass = std::make_unique<render::SpritePass>(std::move(program));
            data_->render.sprite = sprite_pass.release();
        }

        {
            std::unique_ptr<gl::RenderQuad> render_quad = std::make_unique<gl::RenderQuad>();
            data_->render.render_quad = render_quad.release();
        }

        data_->render.initialized = true;

        auto x = [this](const window::size& size) {
            glm::vec2 s(size.width, size.height);
            constexpr float zFar = 1000.f;
            constexpr float zNear = 0.1f;

            glViewport(0, 0, size.width, size.height);
            data_->render.proj = glm::perspective(glm::radians(75.f), s.x / s.y, zNear, zFar);

            const float min_dim = std::min(s.x, s.y);
            data_->render.sprite_proj = glm::ortho(
                (min_dim - s.x) / 2.f / min_dim,
                (min_dim - ((min_dim - s.x) / 2.f)) / min_dim,
                (min_dim - s.y) / 2.f / min_dim,
                (min_dim - ((min_dim - s.y) / 2.f)) / min_dim);

            data_->render.gbuffer->UpdateBuffers(size.width, size.height);
        };

        data_->render.window_resize_conn = window->on_resize.connect(x);
        x(window->GetSize());

        // depth test
        glEnable(GL_DEPTH_TEST);

        // culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);

        assert(data_->render.gbuffer != nullptr);
        assert(data_->render.lighting != nullptr);
        assert(data_->render.block_select != nullptr);
        assert(data_->render.render_quad != nullptr);

        return Ok();
    });
}

} // namespace chunklands::engine