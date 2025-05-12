#pragma once

#include "collectable.hpp"
#include "family.hpp"
#include "labels.hpp"
#include "metric_family.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace postboy::prometheus {

class Counter;
class Gauge;
class Histogram;

namespace detail {

template <typename T>
class Builder;

} // namespace detail

class Registry : public Collectable {
public:
    explicit Registry();

    Registry(const Registry&) = delete;

    Registry& operator=(const Registry&) = delete;

    Registry(Registry&&) = delete;

    Registry& operator=(Registry&&) = delete;

    ~Registry() override;

    std::vector<MetricFamily> Collect() const override;

    template <typename T>
    bool Remove(const Family<T>& family);

private:
    template <typename T>
    friend class detail::Builder;

    template <typename T>
    std::vector<std::unique_ptr<Family<T>>>& GetFamilies();

    template <typename T>
    bool NameExistsInOtherType(const std::string& name) const;

    template <typename T>
    Family<T>& Add(const std::string& name, const std::string& help, const Labels& labels);

    std::vector<std::unique_ptr<Family<Counter>>> counters_;
    std::vector<std::unique_ptr<Family<Gauge>>> gauges_;
    std::vector<std::unique_ptr<Family<Histogram>>> histograms_;
    mutable std::mutex mutex_;
};

} // namespace postboy::prometheus
