#pragma once

#include <postboy/prometheus_metrics/labels.hpp>

#include <string>

namespace postboy::prometheus {

template <typename T>
class Family;
class Registry;

namespace detail {

template <typename T>
class Builder {
public:
    Builder& Labels(const postboy::prometheus::Labels& labels);
    Builder& Name(const std::string& name);
    Builder& Help(const std::string& help);
    Family<T>& Register(Registry& registry);

private:
    postboy::prometheus::Labels labels_;
    std::string name_;
    std::string help_;
};

} // namespace detail

} // namespace postboy::prometheus
