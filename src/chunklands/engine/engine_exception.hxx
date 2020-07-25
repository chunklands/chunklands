#ifndef __CHUNKLANDS_ENGINE_ENGINE_EXCEPTION_HXX__
#define __CHUNKLANDS_ENGINE_ENGINE_EXCEPTION_HXX__

#include <backward.hpp>
#include <boost/exception/exception.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <string>

#define ENGINE_CHECK(api_function)

namespace chunklands::engine {

  struct engine_exception : virtual std::exception, virtual boost::exception {};

  struct tag_stacktrace;
  using traced = boost::error_info<struct tag_stacktrace, backward::StackTrace>;

  inline BOOST_NORETURN void throw_engine_exception(const char* api_function) {
    backward::StackTrace stack;
    stack.load_here(32);
    BOOST_THROW_EXCEPTION(engine_exception()
      << traced(std::move(stack))
      << boost::errinfo_api_function(api_function)
    );
  }

} // namespace chunklands::engine

#endif