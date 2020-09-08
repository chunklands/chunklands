#ifndef __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__
#define __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__

#include <atomic>
#include <chunklands/engine/Engine.hxx>
#include <chunklands/libcxx/boost_thread.hxx>
#include <chunklands/libcxx/easylogging++.hxx>
#include <chunklands/libcxx/js.hxx>
#include <iostream>
#include <memory>
#include <thread>

namespace chunklands::core {

constexpr bool log = false;

class EngineBridge : public JSObjectWrap<EngineBridge> {
    JS_DECL_INITCTOR()

    JS_DECL_CB_VOID(terminate)
    JS_DECL_CB_VOID(startProfiling)
    JS_DECL_CB_VOID(stopProfiling)

    // GLFW
    JS_DECL_CB(GLFWInit)
    JS_DECL_CB_VOID(GLFWStartPollEvents)

    // window
    JS_DECL_CB(windowCreate)
    JS_DECL_CB(windowLoadGL)
    JS_DECL_CB(windowOn)
    JS_DECL_CB(windowGetSize)

    // RenderPipeline
    JS_DECL_CB(renderPipelineInit)

    // Block
    JS_DECL_CB(blockCreate)
    JS_DECL_CB(blockBake)

    // Sprite
    JS_DECL_CB(spriteCreate)

    // SpriteInstance
    JS_DECL_CB(spriteInstanceCreate)
    JS_DECL_CB(spriteInstanceUpdate)

    // Chunk
    JS_DECL_CB(chunkCreate)
    JS_DECL_CB(chunkDelete)
    JS_DECL_CB(chunkUpdate)

    // Scene
    JS_DECL_CB(sceneAddChunk)
    JS_DECL_CB(sceneRemoveChunk)

    // Camera
    JS_DECL_CB(cameraAttachWindow)
    JS_DECL_CB(cameraDetachWindow)
    JS_DECL_CB(cameraGetPosition)
    JS_DECL_CB(cameraOn)

    // Character
    JS_DECL_CB_VOID(characterSetCollision)
    JS_DECL_CB(characterIsCollision)
    JS_DECL_CB_VOID(characterSetFlightMode)
    JS_DECL_CB(characterIsFlightMode)

    // Font
    JS_DECL_CB(fontLoad)

    // Text
    JS_DECL_CB(textCreate)
    JS_DECL_CB(textUpdate)

    // Game
    JS_DECL_CB(gameOn)

public:
    EngineBridge(JSCbi info);
    ~EngineBridge();

private:
    template <class T, class F>
    inline void RunInNodeThread(std::unique_ptr<T> data, F&& fn);

    template <class T, class F, class FC>
    JSPromise MakeEngineCall(JSEnv env, engine::AsyncEngineResult<T> async_result, F&& fn, FC&& cleanup);

    template <class T, class F>
    JSPromise MakeEngineCall(JSEnv env, engine::AsyncEngineResult<T> async_result, F&& fn)
    {
        return MakeEngineCall(env, std::move(async_result), std::forward<F>(fn), []() {});
    }

    template <class T, class F, class R = std::result_of_t<F && (boost::future<T>, JSDeferred)>>
    inline JSValue RunInNodeThread(JSEnv env, boost::future<T> result, F&& fn);

    template <class Event, class F, class F2>
    JSValue EventHandler(JSEnv env, JSValue js_type, JSValue js_callback, F&& fn_calls_engine, F2&& fn_result);

private:
    inline bool IsNodeThread() const
    {
        return std::this_thread::get_id() == node_thread_id_;
    }

    inline bool NotIsNodeThread() const
    {
        return !IsNodeThread();
    }

    inline void IncrementJsCalls()
    {
        js_calls++;
    }

    inline void DecrementJsCalls()
    {
        js_calls--;
    }

    inline void LogJsCalls()
    {
        LOG_IF(log, DEBUG) << "jscalls " << (int)js_calls;
    }

private:
    engine::Engine* engine_ = nullptr;
    std::thread::id node_thread_id_;
    JSTSFunction fn_;
    std::atomic<int> js_calls { 0 };
};

} // namespace chunklands::core

#include "EngineBridge.inl"

#endif