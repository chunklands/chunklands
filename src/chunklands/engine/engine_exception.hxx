#ifndef __CHUNKLANDS_ENGINE_ENGINEEXCEPTION_HXX__
#define __CHUNKLANDS_ENGINE_ENGINEEXCEPTION_HXX__

#include <boost/exception/all.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/exception.hpp>
#include <chunklands/libcxx/exception.hxx>
#include <sstream>
#include <string>

namespace chunklands::engine {

struct engine_exception : virtual std::exception, virtual boost::exception {
    static const char* name()
    {
        return "engine_exception";
    }
};

inline engine_exception create_engine_exception(const char* api_function)
{
    return engine_exception()
        << boost::errinfo_api_function(api_function)
        << libcxx::exception::create_errinfo_stacktrace(1);
}

inline engine_exception create_engine_exception(const char* api_function, std::string message)
{
    return engine_exception()
        << boost::errinfo_api_function(api_function)
        << libcxx::exception::create_errinfo_stacktrace(1)
        << libcxx::exception::messaged(std::move(message));
}

inline std::string get_engine_exception_message(const engine_exception& e)
{
    std::ostringstream ss;
    libcxx::exception::add_exception_name(ss, e);
    libcxx::exception::add_api_function_message(ss, e);
    libcxx::exception::add_stacktrace_message(ss, e);

    return ss.str();
}

} // namespace chunklands::engine

#endif