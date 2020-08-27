#ifndef __CHUNKLANDS_LIBCXX_EASYLOGGINGPP_HXX__
#define __CHUNKLANDS_LIBCXX_EASYLOGGINGPP_HXX__

#define ELPP_THREAD_SAFE
#define ELPP_FEATURE_PERFORMANCE_TRACKING
#define ELPP_NO_DEFAULT_LOG_FILE
#include <easylogging++.h>

#define LOG_DESTRUCT(CLASS) LOG_IF(log, DEBUG) << "destruct " #CLASS

#define LOG_PROCESS(STR) chunklands::libcxx::easyloggingpp::log_process __log_process(STR)

namespace chunklands::libcxx::easyloggingpp {

constexpr bool log = true;

struct log_process {
    log_process(const char* str)
        : str(str)
    {
        LOG_IF(log, DEBUG) << str << ": pending...";
    }

    ~log_process()
    {
        LOG_IF(log, DEBUG) << str << ": done";
    }

    const char* str;
};

} // namespace chunklands::libcxx::easyloggingpp

#endif