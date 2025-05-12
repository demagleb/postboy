#pragma once

#include <postboy/prometheus_metrics/metric_family.hpp>

#include <postboy/prometheus_metrics/proto/import_metrics.pb.h>
#include <postboy/prometheus_metrics/proto/metric_family.pb.h>

namespace postboy::prometheus {

class ProtoSerializer {
public:
    import_metrics::ImportMetrics Serialize(const std::vector<MetricFamily>& families) const;

private:
    std::vector<import_metrics::MetricFamily> SerializeMetricFamilies(const std::vector<MetricFamily>& families) const;

    import_metrics::MetricFamily SerializeMetricFamily(const MetricFamily& family) const;
};

} // namespace postboy::prometheus
