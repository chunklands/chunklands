#ifndef __CHUNKLANDS_EXCEPTION_H__
#define __CHUNKLANDS_EXCEPTION_H__

#include <sstream>
#include <stdexcept>
#include <boost/current_function.hpp>
#include "boost_stacktrace_import.h"



#define THROW_RT(MSG)                       \
  do {                                      \
    std::stringstream ss;                   \
    ss << (MSG);                            \
    ss << "\n\n";                           \
    ss << boost::stacktrace::stacktrace();  \
                                            \
    throw std::runtime_error(ss.str());     \
  } while (0)


#define CC_ASSERT(COND) CC_ASSERT_MSG(COND, "failed condition " #COND)

#define CC_ASSERT_MSG(COND, MSG) \
  do { \
    if (!(COND)) { \
      CC_THROW(MSG); \
    } \
  } while (0)

#define CC_THROW(MSG)                \
  do {                                    \
    std::string msg;                      \
    msg += __FILE__;                      \
    msg += ":";                           \
    msg += BOOST_PP_STRINGIZE(__LINE__);  \
    msg += " at ";                        \
    msg += BOOST_CURRENT_FUNCTION;        \
    msg += ": ";                          \
    msg += (MSG);                         \
    throw std::runtime_error(msg);   \
  } while (0)

#endif
