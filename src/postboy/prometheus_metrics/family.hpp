#pragma once

#include "client_metric.hpp"
#include "collectable.hpp"
#include "labels.hpp"
#include "metric_family.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace postboy::prometheus {

/// \tparam T One of the metric types Counter, Gauge, Histogram or Summary.
template <typename T>
class Family : public Collectable {
public:
    /// \brief Create a new metric.
    ///
    /// \param name Set the metric name.
    /// \param help Set an additional description.
    /// \param constant_labels Assign a set of key-value pairs (= labels) to the
    /// metric. All these labels are propagated to each time series within the
    /// metric.
    /// \throw std::runtime_exception on invalid metric or label names.
    Family(const std::string& name, const std::string& help, const Labels& constantLabels);

    /// \brief Add a new dimensional data.
    ///
    /// \param labels Assign a set of key-value pairs (= labels) to the
    /// dimensional data. The function does nothing, if the same set of labels
    /// already exists.
    /// \param args Arguments are passed to the constructor of metric type T. See
    /// Counter, Gauge, Histogram or Summary for required constructor arguments.
    /// \return Return the newly created dimensional data.
    ///
    /// \throw std::runtime_exception on invalid label names or existing labels.
    template <typename... Args>
    T& Add(const Labels& labels, Args&&... args) {
        return Add(labels, std::make_unique<T>(args...));
    }

    /// \brief Remove the given dimensional data.
    ///
    /// \param metric Dimensional data to be removed. The function does nothing,
    /// if the given metric was not returned by Add().
    void Remove(T* metric);

    /// \brief Returns true if the dimensional data with the given labels exist
    ///
    /// \param labels A set of key-value pairs (= labels) of the dimensional data.
    bool Has(const Labels& labels) const;

    /// \brief Returns the name for this family.
    ///
    /// \return The family name.
    const std::string& GetName() const;

    /// \brief Returns the constant labels for this family.
    ///
    /// \return All constant labels as key-value pairs.
    const Labels& GetConstantLabels() const;

    /// \brief Returns the current value of each dimensional data.
    ///
    /// Collect is called by the Registry when collecting metrics.
    ///
    /// \return Zero or more samples for each dimensional data.
    std::vector<MetricFamily> Collect() const override;

private:
    std::vector<std::pair<Labels, std::unique_ptr<T>>> metrics_;

    const std::string name_;
    const std::string help_;
    const Labels constantLabels_;
    mutable std::mutex mutex_;

    ClientMetric CollectMetric(const Labels& labels, T* metric) const;
    T& Add(const Labels& labels, std::unique_ptr<T> object);
};

} // namespace postboy::prometheus
