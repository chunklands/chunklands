#include "js.hxx"
// #define BOOST_STACKTRACE_USE_BACKTRACE
#include <boost/stacktrace.hpp>

namespace chunklands {
  JSError js_create_error(JSEnv env, const std::string& msg) {
    std::stringstream ss;
    ss
      << msg << "\n"
      << "\n"
      << "    C++ >>>\n"
      << "\n";

    for (auto&& frame : boost::stacktrace::stacktrace(1, 100)) {
      auto&& name = frame.name();
      if (name.rfind("Napi::ObjectWrap<") == 0) {
        break;
      }

      ss << "    at " << frame.name() << "(" << frame.source_file() << ":" << frame.source_line() << ")\n";
    }

    ss
      << "\n"
      << "\n"
      << "    JS  >>>\n";


    return JSError::New(env, ss.str());
  }
}