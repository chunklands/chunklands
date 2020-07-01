#ifndef __CHUNKLANDS_MISC_FLOW_HXX__
#define __CHUNKLANDS_MISC_FLOW_HXX__

#include <stdexcept>
#include <sstream>
#include <boost/current_function.hpp>
#include <chunklands/boost_stacktrace.hxx>

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

#define CHECK(X) do { if (!(X)) { \
    ::chunklands::misc::throw_rt("failed: " #X); \
  }} while (0)

#define CHECK_MSG(X, MSG) do { if (!(X)) {  \
    std::string msg = "failed: " #X;        \
    msg += ", message: ";                   \
    msg += (MSG);                           \
    ::chunklands::misc::throw_rt(msg);      \
  }} while (0)

#include <memory>

namespace chunklands::misc {

  namespace {
    template<typename T>
    inline void write_ss(std::stringstream& ss, T&& arg) {
      ss << std::forward<T>(arg);
    }

    template<typename T, typename... Args>
    inline void write_ss(std::stringstream& ss, T&& arg, Args&&... args) {
      write_ss(ss, arg);
      write_ss(ss, args...);
    }
  }

  template<typename... Args>
  [[noreturn]] void throw_rt(Args&&... args) {
    std::stringstream ss;
    write_ss(ss, std::forward<Args>(args)...);
    ss << "\n\n";
    ss << boost::stacktrace::stacktrace(1, std::numeric_limits<std::size_t>().max());
    
    throw std::runtime_error(ss.str());
  }

} // namespace chunklands::misc

#endif