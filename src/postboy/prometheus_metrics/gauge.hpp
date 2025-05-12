#pragma once

#include "client_metric.hpp"
#include "details/builder.hpp"
#include "metric_types.hpp"

#include <common/util/types.hpp>

#include <atomic>

namespace postboy::prometheus {

class Gauge {
public:
    Gauge()
        : value_(0) {
    }

    FORCE_INLINE void Increment() {
        Change(1.0);
    }

    FORCE_INLINE void Increment(const double value) {
        Change(value);
    }

    FORCE_INLINE void Decrement() {
        Change(-1.0);
    }

    FORCE_INLINE void Decrement(const double value) {
        Change(-value);
    }

    FORCE_INLINE void Set(const double value) {
        value_.store(value, std::memory_order_release);
    }

    FORCE_INLINE ClientMetric Collect() const {
        ClientMetric metric;
        metric.gauge.value = value_.load(std::memory_order_acquire);
        return metric;
    }

    static constexpr MetricType MetricType() {
        return MetricType::Gauge;
    }

private:
    FORCE_INLINE void Change(const double value) {
        value_.fetch_add(value, std::memory_order_release);
    }

private:
    std::atomic<double> value_;
};

using GaugePtr = Gauge*;

detail::Builder<Gauge> BuildGauge();

} // namespace postboy::prometheus
