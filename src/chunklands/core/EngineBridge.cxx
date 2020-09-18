
#include "EngineBridge.hxx"
#include <chunklands/libcxx/easy_profiler.hxx>
#include <chunklands/libcxx/easylogging++.hxx>
#include <tuple>

namespace chunklands::core {

EngineBridge::EngineBridge(JSCbi info)
    : JSObjectWrap<EngineBridge>(info)
{
    engine_ = new engine::Engine();
    fn_ = JSTSFunction::New(info.Env(), JSFunction::New(
                                            info.Env(), [](JSCbi) {}, "dummy"),
        "EngineBridgeCallback", 0, 1);

    node_thread_id_ = std::this_thread::get_id();
}

EngineBridge::~EngineBridge()
{
    delete engine_;
    engine_ = nullptr;
}

JS_DEF_INITCTOR(EngineBridge, ONE_ARG({
                                  JS_CB(terminate),
                                  JS_CB(startProfiling),
                                  JS_CB(stopProfiling),
                                  JS_CB(GLFWInit),
                                  JS_CB(GLFWPollEvents),
                                  JS_CB(windowCreate),
                                  JS_CB(windowLoadGL),
                                  JS_CB(windowOn),
                                  JS_CB(windowGetSize),
                                  JS_CB(windowGetContentSize),
                                  JS_CB(renderPipelineInit),
                                  JS_CB(blockCreate),
                                  JS_CB(blockBake),
                                  JS_CB(spriteCreate),
                                  JS_CB(spriteInstanceCreate),
                                  JS_CB(spriteInstanceUpdate),
                                  JS_CB(chunkCreate),
                                  JS_CB(chunkDelete),
                                  JS_CB(chunkUpdate),
                                  JS_CB(sceneAddChunk),
                                  JS_CB(sceneRemoveChunk),
                                  JS_CB(cameraAttachWindow),
                                  JS_CB(cameraDetachWindow),
                                  JS_CB(cameraGetPosition),
                                  JS_CB(cameraOn),
                                  JS_CB(characterSetCollision),
                                  JS_CB(characterIsCollision),
                                  JS_CB(characterSetFlightMode),
                                  JS_CB(characterIsFlightMode),
                                  JS_CB(fontLoad),
                                  JS_CB(textCreate),
                                  JS_CB(textUpdate),
                                  JS_CB(gameOn),
                              }))

void EngineBridge::JSCall_terminate(JSCbi)
{
    LOG_PROCESS("EngineBridge terminate");
    engine_->Terminate();
    fn_.Release();
}

void EngineBridge::JSCall_startProfiling(JSCbi)
{
    EASY_PROFILER_ENABLE;
    EASY_MAIN_THREAD;
    profiler::startListen();
}

void EngineBridge::JSCall_stopProfiling(JSCbi info)
{
    EASY_PROFILER_DISABLE;
    if (info[0].IsString()) {
        std::string filename = info[0].ToString();
        profiler::dumpBlocksToFile(filename.data());
    }
}

} // namespace chunklands::core