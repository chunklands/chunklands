
#include "Engine.hxx"
#include "EngineData.hxx"
#include <chunklands/engine/chunk/Chunk.hxx>
#include <chunklands/engine/chunk/ChunkMeshDataGenerator.hxx>
#include <chunklands/engine/gl/RenderQuad.hxx>
#include <chunklands/engine/render/GBufferPass.hxx>
#include <chunklands/engine/render/LightingPass.hxx>
#include <chunklands/engine/window/Window.hxx>
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/easylogging++.hxx>
#include <iostream>

#define CHUNKLANDS_DEBUG_SWAP_TIME

namespace chunklands::engine {

constexpr bool log = false;

Engine::Engine()
{
    data_ = new EngineData();

    data_->executors.opengl_thread = std::thread([this]() {
        EASY_THREAD("EngineThread");
        el::Helpers::setThreadName("ce");

        {
            LOG_PROCESS("game loop");

            const double now = glfwGetTime();
            double last_update = now;
            double last_render = now;

            while (!data_->executors.stop) {
                const double loop_start = glfwGetTime();

                EASY_BLOCK("GameLoop");

                const double refresh_interval = data_->gameloop.render_refresh_rate == -1 ? 0.1 : 1.0 / double(data_->gameloop.render_refresh_rate);

                {
                    EASY_BLOCK("Engine Tick");
                    const double render_diff = loop_start - last_render;
                    this->Render(render_diff, loop_start);
                    last_render = loop_start;

                    // data_->executors.opengl.run_queued_closures();
                    const double swap_time = loop_start + refresh_interval * 0.75; // e.g. 60Hz -> 16ms

                    const bool has_more_elements = data_->executors.opengl.reschedule_until([&]() {
                        return glfwGetTime() >= swap_time;
                    });

                    LOG_IF(!has_more_elements && log, DEBUG) << "OpenGL executor is empty";

                    this->Update(loop_start - last_update, loop_start);
                    last_update = loop_start;

#ifdef CHUNKLANDS_DEBUG_SWAP_TIME
                    const double render_swap_start = glfwGetTime();
#endif
                    this->RenderSwap(render_diff, loop_start);
#ifdef CHUNKLANDS_DEBUG_SWAP_TIME
                    const double render_swap_end = glfwGetTime();
                    LOG_IF(log, DEBUG) << "wait for swap: " << render_swap_end - render_swap_start << "s";
#endif
                }
            }
        }

        {
            LOG_PROCESS("close loop executor");
            data_->executors.opengl.close();

            this->FinalizeOpenGLThread();
        }
    });
}

Engine::~Engine()
{
    LOG_DESTRUCT(Engine);

    {
        LOG_PROCESS("delete engine data");
        delete data_;
    }
}

void Engine::Render(double diff, double now)
{
    // TODO(daaitch): unnecessary glClear should be removed when render pipeline in place

    const glm::mat4 view = glm::lookAt(data_->camera.camera.GetEye(), data_->camera.camera.GetCenter(), glm::vec3(0, 1, 0));

    {
        EASY_BLOCK("glClear");
        for (const window::Window* const window : data_->window.windows) {
            assert(window != nullptr);

            if (window->IsGLLoaded()) {
                glClearColor(0, 0, 0, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
        }
    }

    {
        glm::vec2 move(0, 0);
        glm::vec2 look(0, 0);
        bool jump = false;

        if (data_->window.current_window_input_controller) {
            const window::interaction_delta interaction = data_->window.current_window_input_controller->GetAndResetInteraction(now);

            move = glm::vec2(interaction.move.forward - interaction.move.back,
                interaction.move.right - interaction.move.left);
            look = glm::vec2(interaction.cursor.dx, interaction.cursor.dy);
            jump = interaction.jump;
        }

        data_->character_controller.MoveAndLook(diff, now, move, look, jump);
    }

    if (data_->render.initialized) {

        {
            EASY_BLOCK("GBufferPass");
            data_->render.gbuffer->BeginPass(data_->render.proj,
                view, data_->camera.camera.GetEye());

            for (const chunk::Chunk* const chunk : data_->chunk.SceneChunks()) {
                assert(chunk);

                if (chunk->state == chunk::ChunkState::kMeshPrepared) {
                    const float linear_new_factor = std::max(1.f - float((now - chunk->mesh_time) / 0.3f), 0.f);
                    data_->render.gbuffer->SetNewFactor(linear_new_factor * linear_new_factor * linear_new_factor * linear_new_factor);
                    chunk->mesh.vao.Render();
                }
            }

            data_->render.gbuffer->EndPass();
        }

        {
            EASY_BLOCK("LightingPass");
            data_->render.lighting->BeginPass(data_->render.gbuffer->GetPositionTexture(),
                data_->render.gbuffer->GetNormalTexture(),
                data_->render.gbuffer->GetColorTexture());

            data_->render.render_quad->Render();
            data_->render.lighting->EndPass();
        }

        if (data_->render.pointing_block) {
            EASY_BLOCK("SelectBlockPass");
            data_->render.block_select->MakePass(data_->render.proj, view, *data_->render.pointing_block);
        }

        {
            EASY_BLOCK("SpritePass");
            data_->render.sprite->BeginPass(data_->render.sprite_proj, data_->render.gbuffer->GetBakeTexture());
            data_->sprite.crosshair.Render();
            data_->sprite.items.Render();
            data_->render.sprite->EndPass();
        }

        {
            EASY_BLOCK("TextPass");
            data_->render.text->BeginPass(data_->render.text_proj);
            for (const text::Text* const text : data_->text.texts) {

                font::Font* font = nullptr;
                if (get_handle(&font, data_->font.fonts, text->font)) {
                    assert(font != nullptr);

                    data_->render.text->SetTexture(font->texture->GetTexture());
                    data_->render.text->SetPosition(text->pos);
                    text->vao.Render();
                }
            }
            data_->render.text->EndPass();
        }
    }

    if (data_->glfw.start_poll_events) {
        EASY_BLOCK("glfwPollEvents");
        glfwPollEvents();
    }

    {
        EASY_BLOCK("processEvents");
        data_->camera.camera.ProcessEvents();
    }
}

void Engine::RenderSwap(double, double)
{
    {
        EASY_BLOCK("glfwSwapBuffers");
        for (window::Window* const window : data_->window.windows) {
            assert(window != nullptr);

            if (window->IsGLLoaded()) {
                window->SwapBuffers();
            }
        }
    }
}

void Engine::Update(double, double)
{
    data_->chunk.UpdateChunks();
}

void Engine::Terminate()
{

    if (!data_->executors.stop) {

        LOG_IF(log, DEBUG) << "set Engine stop = true";
        data_->executors.stop = true;

        {
            LOG_PROCESS("wait for thread join");
            data_->executors.opengl_thread.join();
        }
    }
}

void Engine::FinalizeOpenGLThread()
{
    data_->sprite.crosshair.Clear();
    data_->sprite.items.Clear();
}

} // namespace chunklands::engine