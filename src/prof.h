#ifndef __CHUNKLANDS_PROF_H__
#define __CHUNKLANDS_PROF_H__

#include <boost/chrono.hpp>
#include "ProfilerBase.h"

#define PROF() ::chunklands::prof profiler(BOOST_CURRENT_FUNCTION)
#define PROF_NAME(NAME) ::chunklands::prof profiler(NAME)
#define PROF_MOVE() std::move(profiler)

namespace chunklands {
  using clock = boost::chrono::high_resolution_clock;

  struct prof {
    prof() = delete;
    prof(const prof&) = delete;

    prof(const char* name) : name_(name) {
      if (ProfilerBase::profile_) {
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
      if (ProfilerBase::profile_ && name_ != nullptr) {
        clock::duration delta = clock::now() - start_;
        auto&& micros = boost::chrono::duration_cast<boost::chrono::microseconds>(delta);

        ProfilerBase::AddMeassurement(name_, micros.count());
      }
    }

    clock::time_point start_;
    const char* name_ = nullptr;
  };
}

#endif