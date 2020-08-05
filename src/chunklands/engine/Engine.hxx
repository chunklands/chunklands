#ifndef __CHUNKLANDS_ENGINE_ENGINE_HXX__
#define __CHUNKLANDS_ENGINE_ENGINE_HXX__

#include <chunklands/libcxx/boost_thread.hxx>
#include <thread>

#include "Api.hxx"

namespace chunklands::engine {

  class Engine {
  public:
    Engine();
    ~Engine();

    void Terminate();

    inline Api* GetApi() {
      assert(api_);
      return api_;
    }

  private:
    bool stop_ = false;
    std::thread thread_;
    boost::loop_executor loop_;
    boost::serial_executor serial_;
    Api* api_ = nullptr;
  };

} // namespace chunklands::engine

#endif