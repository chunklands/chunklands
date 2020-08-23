#include <chunklands/core/init.hxx>
#include <chunklands/libcxx/easylogging++.hxx>

using namespace chunklands;

JSObject InitModule(JSEnv env, JSObject exports)
{
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "[%thread] %levshort: %msg");

    el::Helpers::setThreadName("js");

    chunklands::Init(env, exports);
    return exports;
}

NODE_API_MODULE(chunklands, InitModule)
