#pragma once

#include "metric_types.hpp"

#include <string>

namespace postboy::prometheus {

bool CheckMetricName(const std::string& name);
bool CheckLabelName(const std::string& name, MetricType type);

} // namespace postboy::prometheus
