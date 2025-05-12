#pragma once

#include <atomic>
#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <postboy/prometheus_metrics/registry.hpp>

namespace postboy {

prometheus::Registry* GlobalRegistry();

class IMetrics {
public:
    IMetrics();

protected:
    prometheus::Registry* registry_;
};

class IMetricInitializer {
public:
    IMetricInitializer() = default;

    std::function<void(prometheus::Registry&, const std::string&)> InitMetrics();

protected:
    std::function<void(prometheus::Registry&, const std::string&)> initMetrics_;
};

} // namespace postboy
