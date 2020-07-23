#ifndef __CHUNKLANDS_ENGINE_ENGINE_HXX__
#define __CHUNKLANDS_ENGINE_ENGINE_HXX__

#include "Api.hxx"
#include "GameLoop.hxx"

namespace chunklands::engine {

  class Engine {
  public:
    Engine();

    void Init();
    void Terminate();

    std::unique_ptr<Api> api;

  private:
    std::unique_ptr<GameLoop> loop_;
  };

} // namespace chunklands::engine

#endif