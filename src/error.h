#ifndef __CHUNKLANDS_EXCEPTION_H__
#define __CHUNKLANDS_EXCEPTION_H__

#include <sstream>
#include <stdexcept>
#include <boost/stacktrace.hpp>



#define THROW_RT(MSG)                       \
  do {                                      \
    std::stringstream ss;                   \
    ss << (MSG);                            \
    ss << "\n\n";                           \
    ss << boost::stacktrace::stacktrace();  \
                                            \
    throw std::runtime_error(ss.str());     \
  } while (0)

#endif
