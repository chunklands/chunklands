
#include "Engine.hxx"
#include <iostream>
#include <chunklands/libcxx/easy_profiler.hxx>

namespace chunklands::engine {

  Engine::Engine() : loop_(), serial_(loop_) {
    api_ = new Api(&loop_);
    thread_ = std::thread([this]() {
      EASY_THREAD("EngineThread");
      std::unique_ptr<Api> api(api_);
      while (!stop_) {
        EASY_BLOCK("GameLoop");

        {
          EASY_BLOCK("API Tick");
          api->Tick();
        }

        {
          EASY_BLOCK("Loop Queue");
          loop_.run_queued_closures();
        }
      }

      loop_.close();
    });
  }

  Engine::~Engine() {
    std::cout << "~Engine" << std::endl;
    Terminate();
  }

  void Engine::Terminate() {
    if (!stop_) {
      stop_ = true;
      thread_.join();
    }
  }

} // namespace chunklands::engine