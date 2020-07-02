#ifndef __CHUNKLANDS_STACKTRACE_HXX__
#define __CHUNKLANDS_STACKTRACE_HXX__

#ifdef __APPLE__
#define BOOST_STACKTRACE_USE_NOOP
#endif

// #define BOOST_STACKTRACE_USE_BACKTRACE

#include <boost/stacktrace.hpp>

#endif