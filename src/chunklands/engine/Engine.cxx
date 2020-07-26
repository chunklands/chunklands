
#include "Engine.hxx"
#include <iostream>

namespace chunklands::engine {

  Engine::Engine() : loop_(), serial_(loop_) {
    api_ = new Api(loop_);
    thread_ = std::thread([this]() {
      std::unique_ptr<Api> api(api_);
      while (!stop_) {
        api->Tick();
        loop_.run_queued_closures();
      }

      loop_.close();
    });
  }

  Engine::~Engine() {
    std::cout << "~Engine" << std::endl;
    stop_ = true;
    thread_.join();
  }

} // namespace chunklands::engine