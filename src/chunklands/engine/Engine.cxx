
#include "Engine.hxx"
#include <iostream>
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/easylogging++.hxx>

// #define CHUNKLANDS_DEBUG_SWAP_TIME

namespace chunklands::engine {

  Engine::Engine() {
    api_ = new Api(&loop_);

    thread_ = std::thread([this]() {
      EASY_THREAD("EngineThread");
      el::Helpers::setThreadName("ce");

      {
        LOG_PROCESS("game loop");
        
        

        while (!stop_) {
          const double loop_start = glfwGetTime();

          EASY_BLOCK("GameLoop");

          const int render_refresh_rate = api_->GetRenderRefreshRate();
          const double refresh_interval = render_refresh_rate == -1 ? 0.1 : 1.0 / double(render_refresh_rate);

          {
            EASY_BLOCK("API Tick");
            api_->Render();

            const double swap_time = loop_start + refresh_interval * 0.8; // e.g. 60Hz -> 16ms
            
            while (true) {
              const double now = glfwGetTime();
              if (now >= swap_time) {
                break;
              }

              loop_.run_queued_closures();
              api_->Update();
            }
#ifdef CHUNKLANDS_DEBUG_SWAP_TIME
            const double render_swap_start = glfwGetTime();
#endif
            api_->RenderSwap();
#ifdef CHUNKLANDS_DEBUG_SWAP_TIME
            const double render_swap_end = glfwGetTime();
            LOG(DEBUG) << "wait for swap: " << render_swap_end - render_swap_start << "s";
#endif
          }
        }
      }

      {
        LOG_PROCESS("close loop executor");
        loop_.close();
      }

      delete api_;
      api_ = nullptr;
    });
  }

  Engine::~Engine() {
    LOG_DESTRUCT(Engine);
  }

  void Engine::Terminate() {

    if (!stop_) {
      assert(api_ != nullptr);

      LOG(DEBUG) << "set Engine stop = true";
      stop_ = true;

      {
        LOG_PROCESS("wait for thread join");
        thread_.join();
      }

      assert(api_ == nullptr);
    }
  }

} // namespace chunklands::engine