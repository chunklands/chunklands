
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

// #define CHUNKLANDS_DEBUG_SWAP_TIME

namespace chunklands::engine {

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

                    const double swap_time = loop_start + refresh_interval * 0.8; // e.g. 60Hz -> 16ms

                    while (true) {
                        const double now = glfwGetTime();
                        if (now >= swap_time) {
                            break;
                        }

                        data_->executors.opengl.run_queued_closures();
                        this->Update();
                    }
#ifdef CHUNKLANDS_DEBUG_SWAP_TIME
                    const double render_swap_start = glfwGetTime();
#endif
                    this->RenderSwap();
#ifdef CHUNKLANDS_DEBUG_SWAP_TIME
                    const double render_swap_end = glfwGetTime();
                    LOG(DEBUG) << "wait for swap: " << render_swap_end - render_swap_start << "s";
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

            for (Chunk* const chunk : data_->chunk.scene) {
                assert(chunk);

                if (chunk->state == ChunkState::kMeshPrepared) {
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

    {
        // chunk updates
        std::set<Chunk*> blocked_chunks; // DMA
        for (Chunk* chunk : data_->chunk.by_state[ChunkState::kDataPrepared]) {
            assert(chunk);
            assert(chunk->state == ChunkState::kDataPrepared);

            std::array<const Chunk*, kChunkNeighborCount> neighbors;

            const glm::ivec3 chunk_pos = glm::ivec3(chunk->x, chunk->y, chunk->z);
            auto find_and_set_neighbor_chunk = [&](ChunkNeighbor dir, const glm::ivec3& offset) { // lambda
                auto it = data_->chunk.by_pos.find(chunk_pos + offset);
                if (it != data_->chunk.by_pos.end()) {
                    const Chunk* const neighbor = it->second;
                    if (neighbor->state >= kDataPrepared) {
                        neighbors[dir] = it->second;
                        return true;
                    }
                }

                return false;
            };

            if (
                !find_and_set_neighbor_chunk(kChunkNeighborLeft, glm::ivec3(-1, 0, 0)) || !find_and_set_neighbor_chunk(kChunkNeighborRight, glm::ivec3(1, 0, 0)) || !find_and_set_neighbor_chunk(kChunkNeighborBottom, glm::ivec3(0, -1, 0)) || !find_and_set_neighbor_chunk(kChunkNeighborTop, glm::ivec3(0, 1, 0)) || !find_and_set_neighbor_chunk(kChunkNeighborFront, glm::ivec3(0, 0, -1)) || !find_and_set_neighbor_chunk(kChunkNeighborBack, glm::ivec3(0, 0, 1))) {
                blocked_chunks.insert(chunk); // DMA
                continue;
            }

            ChunkMeshDataGenerator generator(chunk, std::move(neighbors));
            generator();

            chunk->state = ChunkState::kMeshPrepared;
            data_->chunk.by_state[ChunkState::kMeshPrepared].insert(chunk);
            // notice: set is all cleared after the loop
        }

        data_->chunk.by_state[ChunkState::kDataPrepared] = std::move(blocked_chunks);
    }
}

void Engine::Terminate()
{

    if (!data_->executors.stop) {

        LOG(DEBUG) << "set Engine stop = true";
        data_->executors.stop = true;

        {
            LOG_PROCESS("wait for thread join");
            data_->executors.opengl_thread.join();
        }
    }
}

} // namespace chunklands::engine