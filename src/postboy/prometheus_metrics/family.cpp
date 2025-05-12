#include "check_names.hpp"
#include "counter.hpp"
#include "family.hpp"
#include "gauge.hpp"
#include "histogram.hpp"

#include <common/util/assert.hpp>
#include <common/util/time.hpp>

#include <algorithm>
#include <cassert>
#include <map>
#include <stdexcept>
#include <utility>

namespace postboy::prometheus {

template <typename T>
Family<T>::Family(const std::string& name, const std::string& help, const Labels& constantLabels)
    : name_(name)
    , help_(help)
    , constantLabels_(constantLabels) {
    VERIFY(CheckMetricName(name_), "Invalid metric name");

    for (auto& labelPair : constantLabels_) {
        VERIFY(CheckLabelName(labelPair.first, T::MetricType()), "Invalid label name");
    }
}

template <typename T>
T& Family<T>::Add(const Labels& labels, std::unique_ptr<T> object) {
    VERIFY(!Has(labels), "Set of labels already exists");

    std::lock_guard<std::mutex> lock{mutex_};

    metrics_.push_back(std::make_pair(labels, std::move(object)));

    for (auto& labelPair : labels) {
        VERIFY(CheckLabelName(labelPair.first, T::MetricType()), "Invalid label name");
        VERIFY(!constantLabels_.contains(labelPair.first), "Duplicate label name");
    }

    return *(metrics_.back().second);
}

template <typename T>
void Family<T>::Remove(T* metric) {
    std::lock_guard<std::mutex> lock{mutex_};

    for (auto it = metrics_.begin(); it != metrics_.end(); ++it) {
        if (it->second.get() == metric) {
            metrics_.erase(it);
            break;
        }
    }
}

template <typename T>
bool Family<T>::Has(const Labels& labels) const {
    std::lock_guard<std::mutex> lock{mutex_};

    auto it = std::find_if(metrics_.begin(), metrics_.end(),
                           [&labels](const auto& metric) { return metric.first == labels; });

    return it != metrics_.end();
}

template <typename T>
const std::string& Family<T>::GetName() const {
    return name_;
}

template <typename T>
const Labels& Family<T>::GetConstantLabels() const {
    return constantLabels_;
}

template <typename T>
std::vector<MetricFamily> Family<T>::Collect() const {
#ifdef SAFE_PROMETHEUS
    #if SAFE_PROMETHEUS == 1
    std::lock_guard<std::mutex> lock{mutex_};
    #endif
#endif

    if (metrics_.empty()) {
        return {};
    }

    auto family = MetricFamily{};
    family.name = name_;
    family.help = help_;
    family.type = T::MetricType();
    family.metric.reserve(metrics_.size());
    for (const auto& m : metrics_) {
        family.metric.push_back(std::move(CollectMetric(m.first, m.second.get())));
    }
    return {family};
}

template <typename T>
ClientMetric Family<T>::CollectMetric(const Labels& metricLabels, T* metric) const {
    auto collected = metric->Collect();
    collected.label.reserve(constantLabels_.size() + metricLabels.size());
    const auto addLabel = [&collected](const std::pair<std::string, std::string>& labelPair) {
        auto label = ClientMetric::Label{};
        label.name = labelPair.first;
        label.value = labelPair.second;
        collected.label.push_back(std::move(label));
    };
    std::for_each(constantLabels_.cbegin(), constantLabels_.cend(), addLabel);
    std::for_each(metricLabels.cbegin(), metricLabels.cend(), addLabel);
    return collected;
}

template class Family<Counter>;
template class Family<Gauge>;
template class Family<Histogram>;

} // namespace postboy::prometheus
