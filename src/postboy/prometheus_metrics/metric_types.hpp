#pragma once

namespace postboy::prometheus {

enum class MetricType {
    Unexpected = 0,
    Counter = 1,
    Gauge = 2,
    Histogram = 3,
};

} // namespace postboy::prometheus
