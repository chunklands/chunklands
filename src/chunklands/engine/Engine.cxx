
#include "Engine.hxx"
#include <iostream>
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/easylogging++.hxx>

namespace chunklands::engine {

  Engine::Engine() {
    api_ = new Api(&loop_);

    thread_ = std::thread([this]() {
      EASY_THREAD("EngineThread");
      el::Helpers::setThreadName("ce");

      {
        LOG_PROCESS("game loop");
        
        while (!stop_) {
          EASY_BLOCK("GameLoop");

          {
            EASY_BLOCK("API Tick");
            api_->Tick();
          }

          {
            EASY_BLOCK("Loop Queue");
            loop_.run_queued_closures();
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
    LOG(DEBUG) << "terminate engine";

    if (!stop_) {
      assert(api_ != nullptr);

      stop_ = true;
      {
        LOG_PROCESS("wait for thread join");
        thread_.join();
      }
      assert(api_ == nullptr);
    }
  }

} // namespace chunklands::engine