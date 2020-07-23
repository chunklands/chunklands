#ifndef __CHUNKLANDS_MISC_PROFILER_HXX__
#define __CHUNKLANDS_MISC_PROFILER_HXX__

#include <chunklands/js.hxx>
#include <unordered_map>
#include <boost/circular_buffer.hpp>
#include <boost/chrono.hpp>

#define PROF() ::chunklands::misc::prof profiler(BOOST_CURRENT_FUNCTION)
#define PROF_NAME(NAME) ::chunklands::misc::prof profiler(NAME)
#define PROF_MOVE() std::move(profiler)

#define DURATION_METRIC(NAME)         ::chunklands::misc::prof profiler(NAME)
#define HISTOGRAM_METRIC(NAME, VALUE) ::chunklands::misc::Profiler::AddHistogram(NAME, VALUE)
#define VALUE_METRIC(NAME, VALUE)     ::chunklands::misc::Profiler::SetGauge(NAME, VALUE)

namespace chunklands::misc {

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

} // namespace chunklands::misc

#endif