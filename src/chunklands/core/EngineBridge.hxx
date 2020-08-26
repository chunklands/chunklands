#ifndef __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__
#define __CHUNKLANDS_CORE_ENGINEBRIDGE_HXX__

#include <chunklands/core/js.hxx>
#include <chunklands/engine/engine/Engine.hxx>
#include <chunklands/libcxx/boost_thread.hxx>
#include <iostream>
#include <memory>
#include <thread>

#define JS_ENGINE_CHECK(x, env, ...)                                                             \
    do {                                                                                         \
        if (!(x)) {                                                                              \
            std::string function_name(BOOST_CURRENT_FUNCTION);                                   \
            std::string api_name = name_of_jscall(function_name);                                \
            auto e = engine::create_engine_exception(api_name.c_str(), #x);                      \
                                                                                                 \
            Napi::Error::New(env, get_engine_exception_message(e)).ThrowAsJavaScriptException(); \
            return __VA_ARGS__;                                                                  \
        }                                                                                        \
    } while (0)

namespace chunklands::core {

inline std::string name_of_jscall(const std::string& name)
{
    const size_t index_of_jscall = name.find("JSCall_");
    if (index_of_jscall == std::string::npos) {
        return name;
    }

    std::string api_name = name.substr(index_of_jscall + 7);
    const size_t index_of_parenthese = api_name.find("(");
    if (index_of_parenthese == std::string::npos) {
        return api_name;
    }

    return api_name.substr(0, index_of_parenthese);
}

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

    // RenderPipeline
    JS_DECL_CB(renderPipelineInit)

    // Block
    JS_DECL_CB(blockCreate)
    JS_DECL_CB(blockBake)

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

private:
    engine::Engine* engine_ = nullptr;
    std::thread::id node_thread_id_;
    JSTSFunction fn_;
};

} // namespace chunklands::core

#include "EngineBridge.inl"

#endif