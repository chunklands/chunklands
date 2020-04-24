#include "ProfilerBase.h"

namespace chunklands {
  std::unordered_map<const char*, boost::circular_buffer<long>> ProfilerBase::meassurements_;

  JS_DEF_WRAP(ProfilerBase, ONE_ARG({
    JS_CB(getMeassurements)
  }))

  Napi::Value ProfilerBase::JSCall_getMeassurements(const Napi::CallbackInfo& info) {
    auto&& meassurements = Napi::Object::New(info.Env());

    std::for_each(meassurements_.cbegin(), meassurements_.cend(), [&](auto& m) {
      float sum = (float)std::accumulate(m.second.cbegin(), m.second.cend(), 0L) / m.second.size();
      meassurements.Set(m.first, Napi::Number::New(info.Env(), sum));
    });

    return meassurements;
  }

  boost::circular_buffer<long>& ProfilerBase::GetOrCreateMeassurement(const char* name) {
    auto&& it = meassurements_.find(name);
    if (it != meassurements_.end()) {
      return it->second;
    }

    auto&& insert = meassurements_.insert(std::make_pair(name, boost::circular_buffer<long>(100)));
    return insert.first->second;
  }

  void ProfilerBase::AddMeassurement(const char* name, long micros) {
    GetOrCreateMeassurement(name).push_back(micros);
  }
}
