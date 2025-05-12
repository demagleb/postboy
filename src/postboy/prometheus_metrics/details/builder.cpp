#include "builder.hpp"

#include <postboy/prometheus_metrics/counter.hpp>
#include <postboy/prometheus_metrics/gauge.hpp>
#include <postboy/prometheus_metrics/histogram.hpp>
#include <postboy/prometheus_metrics/registry.hpp>

namespace postboy::prometheus {

namespace detail {

template <typename T>
Builder<T>& Builder<T>::Labels(const postboy::prometheus::Labels& labels) {
    labels_ = labels;
    return *this;
}

template <typename T>
Builder<T>& Builder<T>::Name(const std::string& name) {
    name_ = name;
    return *this;
}

template <typename T>
Builder<T>& Builder<T>::Help(const std::string& help) {
    help_ = help;
    return *this;
}

template <typename T>
Family<T>& Builder<T>::Register(Registry& registry) {
    return registry.Add<T>(name_, help_, labels_);
}

template class Builder<Counter>;
template class Builder<Gauge>;
template class Builder<Histogram>;

} // namespace detail

detail::Builder<Counter> BuildCounter() {
    return {};
}
detail::Builder<Gauge> BuildGauge() {
    return {};
}
detail::Builder<Histogram> BuildHistogram() {
    return {};
}

} // namespace postboy::prometheus
