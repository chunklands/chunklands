
#include "Engine.hxx"
#include "shared.hxx"
#include <chunklands/engine/Chunk.hxx>
#include <chunklands/engine/ChunkMeshDataGenerator.hxx>
#include <chunklands/engine/GBufferPass.hxx>
#include <chunklands/engine/LightingPass.hxx>
#include <chunklands/engine/Window.hxx>
#include <chunklands/engine/gl/RenderQuad.hxx>
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

            while (!data_->executors.stop) {
                const double loop_start = glfwGetTime();

                EASY_BLOCK("GameLoop");

                const double refresh_interval = data_->gameloop.render_refresh_rate == -1 ? 0.1 : 1.0 / double(data_->gameloop.render_refresh_rate);

                {
                    EASY_BLOCK("Engine Tick");
                    this->Render();

                    // data_->executors.opengl.run_queued_closures();
                    const double swap_time = loop_start + refresh_interval * 0.75; // e.g. 60Hz -> 16ms

                    int i = 0;
                    const bool has_more_elements = data_->executors.opengl.reschedule_until([&]() {
                        i++;
                        return i >= 10 || glfwGetTime() >= swap_time;
                    });

                    LOG_IF(!has_more_elements && log, DEBUG) << "OpenGL executor is empty";

                    this->Update();

#ifdef CHUNKLANDS_DEBUG_SWAP_TIME
                    const double render_swap_start = glfwGetTime();
#endif
                    this->RenderSwap();
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

void Engine::Render()
{
    // TODO(daaitch): unnecessary glClear should be removed when render pipeline in place

    const double now = glfwGetTime();

    {
        EASY_BLOCK("glClear");
        for (const Window* const window : data_->window.windows) {
            assert(window != nullptr);

            if (window->IsGLLoaded()) {
                glClearColor(0, 0, 0, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
        }
    }

    if (data_->window.current_window_input_controller) {
        constexpr double move_factor = 20.0;
        const windowinputcontroller_cursor_t cursor_delta = data_->window.current_window_input_controller->GetAndResetCursorDelta();
        data_->character_controller.Look(glm::vec2(cursor_delta.dx, cursor_delta.dy));

        const windowinputcontroller_move_t move_delta = data_->window.current_window_input_controller->GetAndResetMoveDelta();
        data_->character_controller.Move(
            move_factor * (move_delta.forward - move_delta.back),
            move_factor * (move_delta.right - move_delta.left));
    }

    if (data_->render_pipeline.gbuffer) {
        data_->render_pipeline.gbuffer->UpdateView(data_->camera.camera.GetEye(),
            data_->camera.camera.GetCenter());
    }

    if (data_->render_pipeline.gbuffer && data_->render_pipeline.lighting && data_->render_pipeline.render_quad) {

        {
            EASY_BLOCK("GBufferPass");
            data_->render_pipeline.gbuffer->BeginPass();
            data_->render_pipeline.gbuffer->SetCameraPos(data_->camera.camera.GetEye());

            for (Chunk* const chunk : data_->chunk.SceneChunks()) {
                assert(chunk);

                if (chunk->state == ChunkState::kMeshPrepared) {
                    const float linear_new_factor = std::max(1.f - float((now - chunk->mesh_time) / 0.3f), 0.f);
                    data_->render_pipeline.gbuffer->SetNewFactor(linear_new_factor * linear_new_factor * linear_new_factor * linear_new_factor);
                    chunk->mesh.vao.Render();
                }
            }

            data_->render_pipeline.gbuffer->EndPass();
        }

        {
            EASY_BLOCK("LightingPass");
            data_->render_pipeline.lighting->BeginPass(data_->render_pipeline.gbuffer->GetPositionTexture(),
                data_->render_pipeline.gbuffer->GetNormalTexture(),
                data_->render_pipeline.gbuffer->GetColorTexture());

            data_->render_pipeline.render_quad->Render();
            data_->render_pipeline.lighting->EndPass();
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

void Engine::RenderSwap()
{
    {
        EASY_BLOCK("glfwSwapBuffers");
        for (Window* const window : data_->window.windows) {
            assert(window != nullptr);

            if (window->IsGLLoaded()) {
                window->SwapBuffers();
            }
        }
    }
}

void Engine::Update()
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

} // namespace chunklands::engine