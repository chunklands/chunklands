#ifndef __CHUNKLANDS_MISC_H__
#define __CHUNKLANDS_MISC_H__

#include <boost/chrono.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/current_function.hpp>
// #define BOOST_STACKTRACE_USE_BACKTRACE
#include <boost/stacktrace.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include <chunklands/js.hxx>

#define PROF() ::chunklands::modules::misc::prof profiler(BOOST_CURRENT_FUNCTION)
#define PROF_NAME(NAME) ::chunklands::modules::misc::prof profiler(NAME)
#define PROF_MOVE() std::move(profiler)

#define DURATION_METRIC(NAME)         ::chunklands::modules::misc::prof profiler(NAME)
#define HISTOGRAM_METRIC(NAME, VALUE) ::chunklands::modules::misc::Profiler::AddHistogram(NAME, VALUE)
#define VALUE_METRIC(NAME, VALUE)     ::chunklands::modules::misc::Profiler::SetGauge(NAME, VALUE)


#define THROW_RT(MSG)                       \
  do {                                      \
    std::stringstream ss;                   \
    ss << (MSG);                            \
    ss << "\n\n";                           \
    ss << boost::stacktrace::stacktrace();  \
                                            \
    throw std::runtime_error(ss.str());     \
  } while (0)


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
    THROW_RT("failed: " #X); \
  }} while (0)

#define CHECK_MSG(X, MSG) do { if (!(X)) {  \
    std::string msg = "failed: " #X;        \
    msg += ", message: ";                   \
    msg += (MSG);                           \
    THROW_RT(msg);                          \
  }} while (0)

namespace chunklands::modules::misc {

  class Profiler : public JSObjectWrap<Profiler> {
    JS_IMPL_WRAP(Profiler, ONE_ARG({
      JS_CB(getMetrics)
    }))

    JS_DECL_CB(getMetrics)

  public:
    static void AddHistogram(const char* name, long micros);
    static void SetGauge(const char* name, long value);

  public:
    const static bool profile_ = true;

  private:
    static std::unordered_map<const char*, std::shared_ptr<boost::circular_buffer<long>>> histograms_;
    static std::unordered_map<const char*, long> gauges_;
  };

  using clock = boost::chrono::high_resolution_clock;

  struct prof {
    prof() = delete;
    prof(const prof&) = delete;

    prof(const char* name) : name_(name) {
      if (Profiler::profile_) {
        start_ = clock::now();
      }
    }

    prof(prof&& other) {
      (*this) = std::move(other);
    }

    prof& operator=(const prof& other) = delete;

    prof& operator=(prof&& other) {
      name_ = other.name_;
      start_ = other.start_;

      other.name_ = nullptr;
      return *this;
    }

    ~prof() {
      if (Profiler::profile_ && name_ != nullptr) {
        clock::duration delta = clock::now() - start_;
        auto&& micros = boost::chrono::duration_cast<boost::chrono::microseconds>(delta);

        Profiler::AddHistogram(name_, micros.count());
      }
    }

    clock::time_point start_;
    const char* name_ = nullptr;
  };

}

namespace std {
  template <typename T,
            typename = enable_if_t<is_scalar<decltype(T::x)>::value>,
            typename = enable_if_t<is_scalar<decltype(T::y)>::value>,
            typename = enable_if_t<is_scalar<decltype(T::z)>::value>
  >
  ostream& operator<<(ostream& os, const T& v) {
    return os << "{" << v.x << ", " << v.y << ", " << v.z << "}";
  }
}

#endif