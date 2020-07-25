#ifndef __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__
#define __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__

#include <chunklands/core/js.hxx>
#include <chunklands/engine/Engine.hxx>
#include "EngineBridge.hxx"
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

  class EngineApiBridge : public JSObjectWrap<EngineApiBridge> {
    JS_DECL_INITCTOR()

    JS_DECL_CB_VOID(terminate)

    // GLFW
    JS_DECL_CB     (GLFWInit)
    JS_DECL_CB_VOID(GLFWStartPollEvents)

    // window
    JS_DECL_CB     (windowCreate)
    JS_DECL_CB     (windowLoadGL)
    JS_DECL_CB     (windowOn)

  public:
    EngineApiBridge(JSCbi info);

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
    JSTSFunction fn_;
    std::thread::id node_thread_id_;
    JSWrapRef<EngineBridge> js_engine_bridge_;
    engine::Api* api_ = nullptr;
  };

} // namespace chunklands::core

#include "EngineApiBridge.inl"

#endif