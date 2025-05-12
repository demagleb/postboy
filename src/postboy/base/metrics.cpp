#include "metrics.hpp"

namespace postboy {

prometheus::Registry* GlobalRegistry() {
    static prometheus::Registry registry;
    return &registry;
}

IMetrics::IMetrics()
    : registry_(GlobalRegistry()) {
}

std::function<void(prometheus::Registry&, const std::string&)> IMetricInitializer::InitMetrics() {
    return initMetrics_;
}

} // namespace postboy
