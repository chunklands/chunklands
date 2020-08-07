
#include "Api.hxx"
#include <chunklands/libcxx/easy_profiler.hxx>
#include <iostream>
#include "Window.hxx"
#include "GBufferPass.hxx"
#include "Chunk.hxx"
#include "LightingPass.hxx"
#include "RenderQuad.hxx"
#include "ChunkMeshDataGenerator.hxx"
#include "api/api-shared.hxx"

namespace chunklands::engine {

  Api::Api(void* executor)
    : executor_(executor)
  {
    data_ = new ApiData();
  }

  Api::~Api() {
    std::cout << "~Api" << std::endl;
    delete reinterpret_cast<ApiData*>(data_);
  }

  void Api::Tick() {
    // TODO(daaitch): unnecessary glClear should be removed when render pipeline in place
    ApiData* api_data = reinterpret_cast<ApiData*>(data_);

    {
      EASY_BLOCK("glClear");
      for (const Window* const window : api_data->windows) {
        assert(window != nullptr);

        if (window->IsGLLoaded()) {
          glClearColor(0, 0, 0, 1.f);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
      }
    }

    if (GLFW_start_poll_events_) {
      EASY_BLOCK("glfwPollEvents");
      glfwPollEvents();
    }

    if (api_data->current_window_input_controller) {
      constexpr double move_factor = 20.0;
      const windowinputcontroller_cursor_t cursor_delta = api_data->current_window_input_controller->GetAndResetCursorDelta();
      api_data->character_controller.Look(glm::vec2(cursor_delta.dx, cursor_delta.dy));

      const windowinputcontroller_move_t move_delta = api_data->current_window_input_controller->GetAndResetMoveDelta();
      api_data->character_controller.Move(
        move_factor * (move_delta.forward - move_delta.back),
        move_factor * (move_delta.right - move_delta.left)
      );
    }

    if (g_buffer_pass_handle_) {
      GBufferPass* g_buffer_pass = reinterpret_cast<GBufferPass*>(g_buffer_pass_handle_);
      g_buffer_pass->UpdateView(api_data->camera.GetEye(), api_data->camera.GetCenter());
    }

    if (g_buffer_pass_handle_ && lighting_pass_handle_ && render_quad_handle_) {
      GBufferPass* g_buffer_pass = reinterpret_cast<GBufferPass*>(g_buffer_pass_handle_);

      {
        EASY_BLOCK("GBufferPass");
        g_buffer_pass->BeginPass();

        for (CEChunkHandle* const chunk_handle : scene_chunks_) {
          assert(chunk_handle);
          Chunk* const chunk = reinterpret_cast<Chunk*>(chunk_handle);
          assert(chunk);

          if (chunk->state == ChunkState::kMeshPrepared) {
            chunk->mesh.vao.Render();
          }
        }

        g_buffer_pass->EndPass();
      }

      {
        EASY_BLOCK("LightingPass");
        LightingPass* lighting_pass = reinterpret_cast<LightingPass*>(lighting_pass_handle_);
        lighting_pass->BeginPass(g_buffer_pass->GetPositionTexture(), g_buffer_pass->GetNormalTexture(), g_buffer_pass->GetColorTexture());
        RenderQuad* render_quad = reinterpret_cast<RenderQuad*>(render_quad_handle_);
        render_quad->Render();
        lighting_pass->EndPass();
      }
    }

    {
      // chunk updates
      for (CEChunkHandle* chunk_handle : chunks_by_state_[ChunkState::kDataPrepared]) {
        assert(chunk_handle);
        Chunk* chunk = reinterpret_cast<Chunk*>(chunk_handle);
        assert(chunk);
        assert(chunk->state == ChunkState::kDataPrepared);
        ChunkMeshDataGenerator generator(chunk);
        generator();

        chunk->state = ChunkState::kMeshPrepared;
        chunks_by_state_[ChunkState::kMeshPrepared].insert(chunk_handle);
        // notice: set is all cleared after the loop
      }

      chunks_by_state_[ChunkState::kDataPrepared].clear();
    }

    {
      EASY_BLOCK("glfwSwapBuffers");
      for (Window* const window : api_data->windows) {
        assert(window != nullptr);

        if (window->IsGLLoaded()) {
          window->SwapBuffers();
        }
      }
    }
  }

} // namespace chunklands::engine