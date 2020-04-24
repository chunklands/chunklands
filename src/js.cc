#include "js.h"
#include "boost_stacktrace_import.h"

namespace chunklands {
  Napi::Error create_error(Napi::Env env, const std::string& msg) {
    std::stringstream ss;
    ss
      << msg << "\n"
      << "\n"
      << "    C++ >>>\n"
      << "\n";

    for (auto&& frame : boost::stacktrace::stacktrace(1, 100)) {
      auto&& name = frame.name();
      if (name.rfind("Napi::ObjectWrap") == 0) {
        break;
      }

      ss << "    at " << frame.name() << "(" << frame.source_file() << ":" << frame.source_line() << ")\n";
    }

    ss
      << "\n"
      << "\n"
      << "    JS  >>>\n";

    return Napi::Error::New(env, ss.str());
  }
}