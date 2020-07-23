#ifndef __CHUNKLANDS_ENGINE_GAMELOOP_HXX__
#define __CHUNKLANDS_ENGINE_GAMELOOP_HXX__

#include <thread>
#include "Api.hxx"
#include <chunklands/libcxx/glfw.hxx>
#include <boost/thread/thread.hpp>
#include <iostream>

namespace chunklands::engine {

  class GameLoop {
  public:
    GameLoop(Api* api);

    void Start();
    void Stop();

  private:
    Api* api_ = nullptr;
    bool stop_ = false;
    std::thread thread_;
  };

} // namespace chunklands::engine

#endif