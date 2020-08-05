#ifndef __CHUNKLANDS_LIBCXX_EXCEPTION_HXX__
#define __CHUNKLANDS_LIBCXX_EXCEPTION_HXX__

#include <boost/exception/all.hpp>
#include <boost/exception/exception.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <sstream>
#define BACKWARD_HAS_BFD 1
#include <backward.hpp>

namespace chunklands::libcxx::exception {

  inline backward::StackTrace load_stacktrace(size_t skip) {
    backward::StackTrace stack_trace;
    stack_trace.load_here(64);
    stack_trace.skip_n_firsts(3 + skip); // skip unwind callback + load_here + load_stacktrace
    return stack_trace;
  }

  struct tag_stacktrace;
  using traced = boost::error_info<struct tag_stacktrace, backward::StackTrace>;

  inline traced create_errinfo_stacktrace(size_t skip) {
    return traced(load_stacktrace(1 + skip)); // skip create_errinfo_stacktrace
  }

  struct tag_message;
  using messaged = boost::error_info<struct tag_message, std::string>;

  template<class E>
  inline void add_exception_name(std::ostream& ss, const E&) {
    ss
      << "=== Exception ===" << std::endl
      << E::name() << std::endl
      << std::endl;
  }

  template<class E>
  inline void add_api_function_message(std::ostream& ss, const E& e) {
    const char* const* const api_function = boost::get_error_info<boost::errinfo_api_function>(e);
    if (!api_function || !*api_function) {
      return;
    }

    ss
      << "=== API-Function ===" << std::endl
      << *api_function << std::endl
      << std::endl;
  }

  template<class E>
  inline void add_message_message(std::ostream& ss, const E& e) {
    const std::string* const message = boost::get_error_info<messaged>(e);
    if (!message) {
      return;
    }

    ss
      << "=== Message ===" << std::endl
      << *message << std::endl
      << std::endl;
  }

  template<class E>
  inline void add_stacktrace_message(std::ostream& ss, const E& e) {
    const backward::StackTrace* const stack_trace = boost::get_error_info<traced>(e);
    if (!stack_trace) {
      return;
    }

    ss << "=== Stacktrace ===" << std::endl;

    backward::Printer printer;
    printer.object = true;
    printer.color_mode = backward::ColorMode::always;
    printer.address = false;
    printer.print(*stack_trace, ss);
    
    ss << std::endl << std::endl;
  }

} // namespace chunklands::libcxx::exception

#endif