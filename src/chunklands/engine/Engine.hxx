#ifndef __CHUNKLANDS_ENGINE_ENGINE_HXX__
#define __CHUNKLANDS_ENGINE_ENGINE_HXX__

#include "Api.hxx"
#include "GameLoop.hxx"

namespace chunklands::engine {

  class Engine {
  public:
    Engine() {
      api = std::make_unique<Api>();
      loop_ = std::make_unique<GameLoop>(api.get());
    }
  public:
    void Init() {
      loop_->Start();
    }

    void Terminate() {
      api->Stop();
      loop_->Stop();
    }

  public:
    std::unique_ptr<Api> api;

  private:
    std::unique_ptr<GameLoop> loop_;
  };

} // namespace chunklands::engine

#endif