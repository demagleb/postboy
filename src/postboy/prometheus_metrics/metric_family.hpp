#pragma once

#include <string>
#include <vector>

#include "client_metric.hpp"
#include "metric_types.hpp"

namespace postboy::prometheus {

struct MetricFamily {
    std::string name;
    std::string help;
    MetricType type = MetricType::Unexpected;
    std::vector<ClientMetric> metric;
};

} // namespace postboy::prometheus
