
#include "Api.hxx"
#include <chunklands/libcxx/easy_profiler.hxx>
#include <iostream>
#include "Window.hxx"
#include "GBufferPass.hxx"
#include "Chunk.hxx"
#include "LightingPass.hxx"
#include "RenderQuad.hxx"
#include "ChunkMeshDataGenerator.hxx"

namespace chunklands::engine {

  Api::Api(void* executor) : executor_(executor) {
  }

  Api::~Api() {
    std::cout << "~Api" << std::endl;
  }

  void Api::Tick() {
    // TODO(daaitch): unnecessary glClear should be removed when render pipeline in place
    {
      EASY_BLOCK("glClear");
      for (const CEWindowHandle* const handle : windows_) {
        const Window* const window = reinterpret_cast<const Window*>(handle);
        assert(window);

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
      for (CEWindowHandle* const handle : windows_) {
        Window* const window = reinterpret_cast<Window*>(handle);
        assert(window);

        if (window->IsGLLoaded()) {
          window->SwapBuffers();
        }
      }
    }
  }

} // namespace chunklands::engine