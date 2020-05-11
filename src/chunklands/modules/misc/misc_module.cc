#include "misc_module.h"

#include <unordered_map>
#include <numeric>

namespace chunklands::modules::misc {
  /////////////////////////////////////////////////////////////////////////////
  // Profiler //////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  std::unordered_map<const char*, boost::circular_buffer<long>> Profiler::meassurements_;

  JS_DEF_WRAP(Profiler)

  JSValue Profiler::JSCall_getMeassurements(JSCbi info) {
    auto&& meassurements = JSObject::New(info.Env());

    std::for_each(meassurements_.cbegin(), meassurements_.cend(), [&](auto& m) {
      float sum = (float)std::accumulate(m.second.cbegin(), m.second.cend(), 0L) / m.second.size();
      meassurements.Set(m.first, JSNumber::New(info.Env(), sum));
    });

    return meassurements;
  }

  boost::circular_buffer<long>& Profiler::GetOrCreateMeassurement(const char* name) {
    auto&& it = meassurements_.find(name);
    if (it != meassurements_.end()) {
      return it->second;
    }

    auto&& insert = meassurements_.insert(std::make_pair(name, boost::circular_buffer<long>(100)));
    return insert.first->second;
  }
}