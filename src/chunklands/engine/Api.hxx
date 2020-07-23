#ifndef __CHUNKLANDS_ENGINE_API_HXX__
#define __CHUNKLANDS_ENGINE_API_HXX__

#define BOOST_THREAD_VERSION 4
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_EXECUTORS
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/executors/loop_executor.hpp>
#include <boost/thread/executors/serial_executor.hpp>
#include <boost/thread/thread.hpp>
#include <boost/signals2.hpp>

namespace chunklands::engine {

  struct WindowHandle;

  class Api {
  public:
    Api() : serial_(loop_) {}
    ~Api() {
      int i = 4;
      assert(i);
    }

  public:
    void RunCommands();
    void Stop() {
      serial_.close();
    }

  public:
    // GLFW
    boost::future<bool>
    GLFWInit();

    void
    GLFWStartPollEvents(bool poll);

    bool
    GLFWStartPollEvents() const {
      return GLFW_start_poll_events;
    }
    

    // Window
    boost::future<WindowHandle*>
    WindowCreate(int width, int height, std::string title);
    
    void
    WindowMakeContextCurrent(WindowHandle* handle);

    boost::signals2::scoped_connection
    WindowOn(WindowHandle* handle, const std::string& event, std::function<void()> callback);
    
  private:
    template<class F, class R = std::result_of_t<F&&()>>
    inline boost::future<R> EnqueueTask(F&& fn) {
      boost::packaged_task<R()> task(std::forward<F>(fn));
      boost::future<R> result = task.get_future();
      serial_.submit(std::move(task));

      return result;
    }

  private:
    boost::loop_executor loop_;
    boost::serial_executor serial_;

    bool GLFW_start_poll_events = false;
  };

} // namespace chunklands::engine

#endif