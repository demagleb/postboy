#pragma once

#include <postboy/base/processor.hpp>
#include <postboy/base/metrics.hpp>

#include <postboy/prometheus_metrics/counter.hpp>

namespace postboy {

class WebsocketMetrics : public IMetricInitializer {
public:
    WebsocketMetrics();

public:
    prometheus::CounterPtr receivePacketCounter_;
};

} // namespace postboy
