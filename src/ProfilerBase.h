#ifndef __CHUNKLANDS_PROFILERBASE_H__
#define __CHUNKLANDS_PROFILERBASE_H__

#include <napi.h>
#include <unordered_map>
#include <boost/circular_buffer.hpp>
#include "napi/object_wrap_util.h"

namespace chunklands {
  class ProfilerBase : public Napi::ObjectWrap<ProfilerBase> {
    DECLARE_OBJECT_WRAP(ProfilerBase)
    DECLARE_OBJECT_WRAP_CB(Napi::Value GetMeassurements)

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
