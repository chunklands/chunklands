#ifndef __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__
#define __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__

#include <chunklands/core/js.hxx>
#include <chunklands/engine/Engine.hxx>
#include "EngineBridge.hxx"
#include <memory>

#include <iostream>

namespace chunklands::core {

  void api_call_void_resolver(JSEnv env, boost::future<void> result, JSDeferred deferred);

  class EngineApiBridge : public JSObjectWrap<EngineApiBridge> {
    JS_DECL_INITCTOR()

    // GLFW
    JS_DECL_CB      (GLFWInit)
    JS_DECL_CB_VOID (GLFWStartPollEvents)

    // window
    JS_DECL_CB      (windowCreate)
    JS_DECL_CB      (windowLoadGL)
    JS_DECL_CB      (windowOn)

    // RenderPipeline
    JS_DECL_CB      (renderPipelineInit)

    // Block
    JS_DECL_CB      (blockCreate)
    JS_DECL_CB      (blockBake)

    // Chunk
    JS_DECL_CB      (chunkCreate)
    JS_DECL_CB      (chunkUpdate)

    // Scene
    JS_DECL_CB      (sceneAddChunk)
    JS_DECL_CB      (sceneRemoveChunk)

    // Camera
    JS_DECL_CB      (cameraAttachWindow)
    JS_DECL_CB      (cameraDetachWindow)
    JS_DECL_CB      (cameraGetPosition)
    JS_DECL_CB      (cameraOn)

  public:
    EngineApiBridge(JSCbi info);
    ~EngineApiBridge();

  private:
    template<class T, class F>
    inline void RunInNodeThread(std::unique_ptr<T> data, F&& fn);

    template<class T, class F>
    JSPromise FromNodeThreadRunApiResultInNodeThread(JSEnv env, boost::future<T> result, F fn);

    template<class T, class F, class R = std::result_of_t<F&&(boost::future<T>, JSDeferred)>>
    inline JSValue RunInNodeThread(JSEnv env, boost::future<T> result, F&& fn);

    template<class Event, class F, class F2>
    JSValue EventHandler(JSEnv env, JSValue js_type, JSValue js_callback, F&& fn_calls_api, F2&& fn_result);

  private:
    inline bool IsNodeThread() const {
      return std::this_thread::get_id() == node_thread_id_;
    }

    inline bool NotIsNodeThread() const {
      return !IsNodeThread();
    }

  private:
    engine::Api* api_ = nullptr;
    std::thread::id node_thread_id_;
    JSWrapRef<EngineBridge> js_engine_bridge_;
    JSTSFunction fn_;
  };

} // namespace chunklands::core

#include "EngineApiBridge.inl"

#endif