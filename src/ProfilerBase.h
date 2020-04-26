#ifndef __CHUNKLANDS_PROFILERBASE_H__
#define __CHUNKLANDS_PROFILERBASE_H__

#include <unordered_map>
#include <boost/circular_buffer.hpp>
#include "js.h"

namespace chunklands {
  class ProfilerBase : public JSObjectWrap<ProfilerBase> {
    JS_IMPL_WRAP(ProfilerBase, ONE_ARG({
      JS_CB(getMeassurements)
    }))

    JS_DECL_CB(getMeassurements)

  public:
    static void AddMeassurement(const char* name, long micros) {
      GetOrCreateMeassurement(name).push_back(micros);
    }
  
  private:
    static boost::circular_buffer<long>& GetOrCreateMeassurement(const char* name);

  public:
    const static bool profile_ = true;

  private:
    static std::unordered_map<const char*, boost::circular_buffer<long>> meassurements_;
  };
}

#endif
