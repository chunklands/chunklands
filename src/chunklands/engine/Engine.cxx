
#include "Engine.hxx"
#include <iostream>
#include <chunklands/libcxx/easy_profiler.hxx>

namespace chunklands::engine {

  Engine::Engine() : loop_() {
    api_ = new Api(&loop_);

    thread_ = std::thread([this]() {
      EASY_THREAD("EngineThread");

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

      loop_.close();
      delete api_;
      api_ = nullptr;
    });
  }

  Engine::~Engine() {
    std::cout << "~Engine" << std::endl;
  }

  void Engine::Terminate() {
    if (!stop_) {
      assert(api_ != nullptr);

      stop_ = true;
      thread_.join();
      assert(api_ == nullptr);
    }
  }

} // namespace chunklands::engine