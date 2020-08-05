
#include "EngineBridge.hxx"
#include <chunklands/libcxx/easy_profiler.hxx>

namespace chunklands::core {

  EngineBridge::EngineBridge(JSCbi info) : JSObjectWrap<EngineBridge>(info) {
    fn_ = JSTSFunction::New(info.Env(), JSFunction::New(info.Env(), [](JSCbi){}, "dummy"), "EngineApiBridgeCallback", 0, 1);
  }

  JS_DEF_INITCTOR(EngineBridge, ONE_ARG({
    JS_CB(terminate),
    JS_CB(startProfiling),
    JS_CB(stopProfiling),
  }))

  void EngineBridge::JSCall_terminate(JSCbi) {
    engine_.Terminate();
    fn_.Release();
  }

  void EngineBridge::JSCall_startProfiling(JSCbi) {
    EASY_PROFILER_ENABLE;
    EASY_MAIN_THREAD;
    profiler::startListen();
  }

  void EngineBridge::JSCall_stopProfiling(JSCbi info) {
    EASY_PROFILER_DISABLE;
    if (info[0].IsString()) {
      std::string filename = info[0].ToString();
      profiler::dumpBlocksToFile(filename.data());
    }
  }

} // namespace chunklands::core