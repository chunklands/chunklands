#ifndef __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__
#define __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__

#include <chunklands/core/js.hxx>
#include <chunklands/engine/Engine.hxx>
#include <memory>
#define BOOST_THREAD_VERSION 4
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_EXECUTORS
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/executors/loop_executor.hpp>
#include <boost/thread/executors/serial_executor.hpp>
#include <boost/thread/thread.hpp>

#include <iostream>

namespace chunklands::core {

  class EngineBridge : public JSObjectWrap<EngineBridge> {
    JS_DECL_INITCTOR()

    // engine
    JS_DECL_CB_VOID(engineShutdown)

    // GLFW
    JS_DECL_CB     (GLFWInit)
    JS_DECL_CB_VOID(GLFWStartPollEvents)

    // window
    JS_DECL_CB     (windowCreate)
    JS_DECL_CB_VOID(windowMakeContextCurrent)
    JS_DECL_CB     (windowOn)

  public:
    EngineBridge(JSCbi info);
    ~EngineBridge();

  private:
    template<class T, class F>
    inline void RunInNodeThread(std::unique_ptr<T> data, F&& fn);

    template<class T, class F>
    JSPromise FromNodeThreadRunApiResultInNodeThread(JSEnv env, boost::future<T> result, F fn);

    template<class T, class F, class R = std::result_of_t<F&&(boost::future<T>, JSDeferred)>>
    inline JSValue RunInNodeThread(JSEnv env, boost::future<T> result, F&& fn);

  private:
    inline bool IsNodeThread() const {
      return std::this_thread::get_id() == node_thread_id_;
    }

    inline bool NotIsNodeThread() const {
      return !IsNodeThread();
    }

  private:
    std::unique_ptr<engine::Engine> engine_;

    JSTSFunction fn_;
    std::thread::id node_thread_id_;
  };

} // namespace chunklands::core

#include "EngineBridge.inl"

#endif