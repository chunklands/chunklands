
#include "Engine.hxx"

namespace chunklands::engine {

  Engine::Engine() {
    api = std::make_unique<Api>();
    loop_ = std::make_unique<GameLoop>(api.get());
  }

  void Engine::Init() {
    loop_->Start();
  }

  void Engine::Terminate() {
    api->Stop();
    loop_->Stop();
  }

} // namespace chunklands::engine