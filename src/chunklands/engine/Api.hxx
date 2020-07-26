#ifndef __CHUNKLANDS_ENGINE_API_HXX__
#define __CHUNKLANDS_ENGINE_API_HXX__

#include <chunklands/libcxx/boost_thread.hxx>
#include <boost/signals2.hpp>
#include <set>
#include <thread>
#include "GBufferPass.hxx"

namespace chunklands::engine {

  struct WindowHandle;

  class Api {
  public:
    Api(boost::loop_executor& executor) : executor_(executor) {}
    ~Api();

  public:
    void Init();
    void Tick();

  public:
    // GLFW
    boost::future<void>
    GLFWInit();

    void
    GLFWStartPollEvents(bool poll);

    bool
    GLFWStartPollEvents() const { return GLFW_start_poll_events_; }
    
    // Window
    boost::future<WindowHandle*>
    WindowCreate(int width, int height, std::string title);

    boost::future<void>
    WindowLoadGL(WindowHandle* handle);

    boost::signals2::scoped_connection
    WindowOn(WindowHandle* handle, const std::string& event, std::function<void()> callback);

    // GBufferPass
    boost::future<void>
    GBufferPassInit(WindowHandle* handle, std::string vertex_shader, std::string fragment_shader);
    
  private:
    bool IsGameLoopThread() const;

    template<class F, class R = std::result_of_t<F&&()>>
    inline boost::future<R> EnqueueTask(F&& fn) {
      boost::packaged_task<R()> task(std::forward<F>(fn));
      boost::future<R> result = task.get_future();
      executor_.submit(std::move(task));

      return result;
    }

  private:
    boost::loop_executor& executor_;

    std::set<WindowHandle*> window_handles_;

    bool GLFW_start_poll_events_ = false;

    std::unique_ptr<GBufferPass> g_buffer_pass_;
  };

} // namespace chunklands::engine

#endif