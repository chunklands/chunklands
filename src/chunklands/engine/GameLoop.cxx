
#include "GameLoop.hxx"

namespace chunklands::engine {

  GameLoop::GameLoop(Api* api)
    : api_(api) {
  }

  void GameLoop::Start() {
    thread_ = std::thread([this](){
      while (!stop_) {
        double t = glfwGetTime();
        assert(api_);
        api_->RunCommands();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        double diff = glfwGetTime() - t;
        std::cout << "DIFF: " << diff << std::endl;
      }
    });
  }

  void GameLoop::Stop() {
    stop_ = true;
    thread_.join();
  }

} // namespace chunklands::engine