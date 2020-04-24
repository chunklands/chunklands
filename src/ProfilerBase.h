#ifndef __CHUNKLANDS_PROFILERBASE_H__
#define __CHUNKLANDS_PROFILERBASE_H__

#include <unordered_map>
#include <boost/circular_buffer.hpp>
#include "js.h"

namespace chunklands {
  class ProfilerBase : public JSWrap<ProfilerBase> {
    JS_DECL_WRAP(ProfilerBase)
    JS_DECL_CB(getMeassurements)

  public:
    static void AddMeassurement(const char* name, long micros);
  
  private:
    static boost::circular_buffer<long>& GetOrCreateMeassurement(const char* name);

  public:
    const static bool profile_ = true;

  private:
    static std::unordered_map<const char*, boost::circular_buffer<long>> meassurements_;
  };
}

#endif
