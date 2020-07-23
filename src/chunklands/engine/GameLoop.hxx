#ifndef __CHUNKLANDS_ENGINE_GAMELOOP_HXX__
#define __CHUNKLANDS_ENGINE_GAMELOOP_HXX__

#include <thread>
#include "Api.hxx"
#include <chunklands/libcxx/glfw.hxx>
#include <boost/thread/thread.hpp>

namespace chunklands::engine {

  class GameLoop {
  public:
    GameLoop(Api* api) : api_(api) {
    }

    ~GameLoop() {
      int i = 1;
      assert(i);
    }

  public:
    void Start() {
      thread_ = std::thread([this](){
        while (!stop_) {
          api_->RunCommands();
          boost::this_thread::yield();
        }
      });
    }

    void Stop() {
      stop_ = true;
      thread_.join();
    }

  private:
    bool stop_ = false;
    std::thread thread_;
    Api* api_;
  };

} // namespace chunklands::engine

#endif