
#include "GameLoop.hxx"

namespace chunklands::engine {

  GameLoop::GameLoop(Api* api)
    : api_(api) {
  }

  void GameLoop::Start() {
    thread_ = std::thread([this](){
      while (!stop_) {
        assert(api_);
        api_->RunCommands();
      }
    });
  }

  void GameLoop::Stop() {
    stop_ = true;
    thread_.join();
  }

} // namespace chunklands::engine