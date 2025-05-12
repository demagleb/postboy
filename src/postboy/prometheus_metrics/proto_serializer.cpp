#include "proto_serializer.hpp"

#include <common/util/types.hpp>

#include <postboy/prometheus_metrics/client_metric.hpp>
#include <postboy/prometheus_metrics/metric_types.hpp>

#include <postboy/prometheus_metrics/proto/metric_family.pb.h>
#include <postboy/prometheus_metrics/proto/metrics/counter.pb.h>
#include <postboy/prometheus_metrics/proto/metrics/gauge.pb.h>

namespace postboy::prometheus {

import_metrics::ImportMetrics ProtoSerializer::Serialize(const std::vector<MetricFamily>& families) const {
    import_metrics::ImportMetrics result;

    if (UNLIKELY(families.empty())) {
        return result;
    }

    result.add_families();
    auto serializedFamilies = SerializeMetricFamilies(families);

    result.mutable_families()->Add(serializedFamilies.begin(), serializedFamilies.end());

    return result;
}

std::vector<import_metrics::MetricFamily>
ProtoSerializer::SerializeMetricFamilies(const std::vector<MetricFamily>& families) const {
    std::vector<import_metrics::MetricFamily> result;
    result.reserve(families.size());

    for (const auto& family : families) {
        result.emplace_back(SerializeMetricFamily(family));
    }

    return result;
}

import_metrics::MetricFamily ProtoSerializer::SerializeMetricFamily(const MetricFamily& family) const {
    import_metrics::MetricFamily result;

    *result.mutable_name() = family.name;
    *result.mutable_help() = family.help;

    switch (family.type) {
        case (MetricType::Counter): {
            for (const auto& metric : family.metric) {
                import_metrics::metrics::Counter counter;

                counter.set_value(metric.counter.value);
                for (const auto& label : metric.label) {
                    auto* newLabel = counter.add_labels();
                    newLabel->set_key(std::move(label.name));
                    newLabel->set_value(std::move(label.value));
                }

                result.mutable_counters()->Add(std::move(counter));
            }
            break;
        }
        case (MetricType::Gauge): {
            for (const auto& metric : family.metric) {
                import_metrics::metrics::Gauge gauge;

                gauge.set_value(metric.gauge.value);
                for (const auto& label : metric.label) {
                    auto* newLabel = gauge.add_labels();
                    newLabel->set_key(std::move(label.name));
                    newLabel->set_value(std::move(label.value));
                }

                result.mutable_gauges()->Add(std::move(gauge));
            }
            break;
        }
    }

    return result;
}

} // namespace postboy::prometheus
