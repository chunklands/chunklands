
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
      g_buffer_pass->UpdateView(api_data->camera.camera.GetEye(), api_data->camera.camera.GetCenter());
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
      const auto& chunks_by_pos = api_data->chunk.by_pos;
      // chunk updates
      std::set<CEChunkHandle*> blocked_chunks; // DMA
      for (CEChunkHandle* chunk_handle : chunks_by_state_[ChunkState::kDataPrepared]) {
        assert(chunk_handle);
        Chunk* chunk = reinterpret_cast<Chunk*>(chunk_handle);
        assert(chunk);
        assert(chunk->state == ChunkState::kDataPrepared);

        std::array<const Chunk*, kChunkNeighborCount> neighbors;

        const glm::ivec3 chunk_pos = glm::ivec3(chunk->x, chunk->y, chunk->z);
        auto find_and_set_neighbor_chunk = [&](ChunkNeighbor dir, const glm::ivec3& offset) { // lambda
          auto it = chunks_by_pos.find(chunk_pos + offset);
          if (it != chunks_by_pos.end()) {
            const Chunk* const neighbor = it->second;
            if (neighbor->state >= kDataPrepared) {
              neighbors[dir] = it->second;
              return true;
            }
          }

          return false;
        };

        if (
          !find_and_set_neighbor_chunk(kChunkNeighborLeft,    glm::ivec3(-1,  0,  0)) ||
          !find_and_set_neighbor_chunk(kChunkNeighborRight,   glm::ivec3( 1,  0,  0)) ||
          !find_and_set_neighbor_chunk(kChunkNeighborBottom,  glm::ivec3( 0, -1,  0)) ||
          !find_and_set_neighbor_chunk(kChunkNeighborTop,     glm::ivec3( 0,  1,  0)) ||
          !find_and_set_neighbor_chunk(kChunkNeighborFront,   glm::ivec3( 0,  0, -1)) ||
          !find_and_set_neighbor_chunk(kChunkNeighborBack,    glm::ivec3( 0,  0,  1))
        ) {
          blocked_chunks.insert(chunk_handle); // DMA
          continue;
        }
        
        ChunkMeshDataGenerator generator(chunk, std::move(neighbors));
        generator();

        chunk->state = ChunkState::kMeshPrepared;
        chunks_by_state_[ChunkState::kMeshPrepared].insert(chunk_handle);
        // notice: set is all cleared after the loop
      }

      chunks_by_state_[ChunkState::kDataPrepared] = std::move(blocked_chunks);
    }

    if (GLFW_start_poll_events_) {
      EASY_BLOCK("glfwPollEvents");
      glfwPollEvents();
    }

    {
      EASY_BLOCK("processEvents");
      api_data->camera.camera.ProcessEvents();
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