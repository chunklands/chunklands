#ifndef __CHUNKLANDS_NAPI_FLOW_H__
#define __CHUNKLANDS_NAPI_FLOW_H__

#include <boost/current_function.hpp>
#include <boost/preprocessor/stringize.hpp>
#include "../napi_import.h"

#warning deprecated

#define THROW_MSG(ENV, MSG, ...)                                \
  do {                                                          \
    std::string msg;                                            \
    msg += __FILE__;                                            \
    msg += ":";                                                 \
    msg += BOOST_PP_STRINGIZE(__LINE__);                        \
    msg += " at ";                                              \
    msg += BOOST_CURRENT_FUNCTION;                              \
    msg += ": ";                                                \
    msg += (MSG);                                               \
    Napi::Error::New((ENV), msg).ThrowAsJavaScriptException();  \
    return __VA_ARGS__;                                         \
  } while (0)

#endif