#include "misc_module.h"

#include <unordered_map>
#include <numeric>

namespace chunklands::modules::misc {
  /////////////////////////////////////////////////////////////////////////////
  // Profiler //////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  std::unordered_map<const char*, std::shared_ptr<boost::circular_buffer<long>>> Profiler::histograms_;
  std::unordered_map<const char*, long> Profiler::gauges_;

  JS_DEF_WRAP(Profiler)

  JSValue Profiler::JSCall_getMetrics(JSCbi info) {
    auto&& metrics = JSObject::New(info.Env());

    for (auto&& histogram_entry : histograms_) {
      float sum = (float)std::accumulate(
        histogram_entry.second->cbegin(),
        histogram_entry.second->cend(),
        0L
      ) / histogram_entry.second->size();
      metrics.Set(histogram_entry.first, JSNumber::New(info.Env(), sum));
    }

    for (auto&& gauge_entry : gauges_) {
      metrics.Set(gauge_entry.first, JSNumber::New(info.Env(), gauge_entry.second));
    }

    return metrics;
  }

  void Profiler::AddHistogram(const char* name, long micros) {
    auto&& it = histograms_.find(name);
    if (it != histograms_.end()) {
      it->second->push_back(micros);
      return;
    }

    auto&& insert = histograms_.insert({name, std::make_shared<boost::circular_buffer<long>>(100)});
    insert.first->second->push_back(micros);
  }

  void Profiler::SetGauge(const char* name, long value) {
    gauges_.insert_or_assign(name, value);
  }
}
