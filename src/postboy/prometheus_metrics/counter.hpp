#pragma once

#include "client_metric.hpp"
#include "details/builder.hpp"
#include "metric_types.hpp"

#include <common/util/types.hpp>

#include <atomic>

namespace postboy::prometheus {

class Counter {
public:
    Counter()
        : value_(0) {
    }

    FORCE_INLINE void Increment() {
        value_.fetch_add(1, std::memory_order_release);
    }

    FORCE_INLINE void Reset() {
        value_.store(0, std::memory_order_release);
    }

    FORCE_INLINE ClientMetric Collect() const {
        ClientMetric metric;
        metric.counter.value = value_.load(std::memory_order_acquire);
        return metric;
    }

    static constexpr MetricType MetricType() {
        return MetricType::Counter;
    }

private:
    std::atomic<double> value_;
};

using CounterPtr = Counter*;

detail::Builder<Counter> BuildCounter();

} // namespace postboy::prometheus
