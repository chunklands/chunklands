#ifndef __CHUNKLANDS_CORE_ENGINEBRIDGEUTIL_HXX__
#define __CHUNKLANDS_CORE_ENGINEBRIDGEUTIL_HXX__

#include <boost/current_function.hpp>
#include <chunklands/engine/engine_exception.hxx>
#include <string>

#define JS_ENGINE_CHECK(x, env, ...)                                                             \
    do {                                                                                         \
        if (!(x)) {                                                                              \
            std::string function_name(BOOST_CURRENT_FUNCTION);                                   \
            std::string api_name = core::name_of_jscall(function_name);                          \
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

} // namespace chunklands::core

#endif