#ifndef __CHUNKLANDS_ENGINE_GL_GL_EXCEPTION_HXX__
#define __CHUNKLANDS_ENGINE_GL_GL_EXCEPTION_HXX__

#include <boost/exception/all.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/exception.hpp>
#include <chunklands/libcxx/exception.hxx>
#include <sstream>
#include <string>

namespace chunklands::engine::gl {

struct gl_exception : virtual std::exception, virtual boost::exception {
    static const char* name()
    {
        return "gl_exception";
    }
};

inline BOOST_NORETURN void throw_gl_exception(const char* gl_function)
{
    BOOST_THROW_EXCEPTION(gl_exception()
        << libcxx::exception::create_errinfo_stacktrace(1)
        << boost::errinfo_api_function(gl_function));
}

inline BOOST_NORETURN void throw_gl_exception(const char* gl_function, std::string message)
{
    BOOST_THROW_EXCEPTION(gl_exception()
        << libcxx::exception::create_errinfo_stacktrace(1)
        << boost::errinfo_api_function(gl_function)
        << libcxx::exception::messaged(std::move(message)));
}

inline std::string get_gl_exception_message(const gl_exception& e)
{
    std::ostringstream ss;
    libcxx::exception::add_exception_name(ss, e);
    libcxx::exception::add_api_function_message(ss, e);
    libcxx::exception::add_message_message(ss, e);
    libcxx::exception::add_stacktrace_message(ss, e);

    return ss.str();
}

} // namespace chunklands::engine::gl

#endif