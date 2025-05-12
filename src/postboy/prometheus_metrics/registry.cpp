#include "counter.hpp"
#include "gauge.hpp"
#include "histogram.hpp"
#include "registry.hpp"

#include <common/util/assert.hpp>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <tuple>

namespace postboy::prometheus {

namespace {

template <typename T>
void CollectAll(std::vector<MetricFamily>& results, const T& families) {
    for (auto&& collectable : families) {
        auto metrics = collectable->Collect();
        results.insert(results.end(), std::make_move_iterator(metrics.begin()), std::make_move_iterator(metrics.end()));
    }
}

bool FamilyNameExists(const std::string&) {
    return false;
}

template <typename T, typename... Args>
bool FamilyNameExists(const std::string& name, const T& families, Args&&... args) {
    auto sameName = [&name](const typename T::value_type& entry) { return name == entry->GetName(); };
    auto exists = std::find_if(std::begin(families), std::end(families), sameName) != std::end(families);

    return exists || FamilyNameExists(name, args...);
}

} // namespace

Registry::Registry() {
}

Registry::~Registry() = default;

std::vector<MetricFamily> Registry::Collect() const {
#ifdef SAFE_PROMETHEUS
    #if SAFE_PROMETHEUS == 1
    std::lock_guard<std::mutex> lock{mutex_};
    #endif
#endif

    auto results = std::vector<MetricFamily>{};

    CollectAll(results, counters_);
    CollectAll(results, gauges_);
    CollectAll(results, histograms_);

    return results;
}

template <>
std::vector<std::unique_ptr<Family<Counter>>>& Registry::GetFamilies() {
    return counters_;
}

template <>
std::vector<std::unique_ptr<Family<Gauge>>>& Registry::GetFamilies() {
    return gauges_;
}

template <>
std::vector<std::unique_ptr<Family<Histogram>>>& Registry::GetFamilies() {
    return histograms_;
}

template <>
bool Registry::NameExistsInOtherType<Counter>(const std::string& name) const {
    return FamilyNameExists(name, gauges_);
}

template <>
bool Registry::NameExistsInOtherType<Gauge>(const std::string& name) const {
    return FamilyNameExists(name, counters_);
}

template <>
bool Registry::NameExistsInOtherType<Histogram>(const std::string& name) const {
    return FamilyNameExists(name, histograms_);
}

template <typename T>
Family<T>& Registry::Add(const std::string& name, const std::string& help, const Labels& labels) {
    std::lock_guard<std::mutex> lock{mutex_};

    VERIFY(!NameExistsInOtherType<T>(name), "Family name already exists with different type");

    auto& families = GetFamilies<T>();

    auto sameName = [&name](const std::unique_ptr<Family<T>>& family) { return name == family->GetName(); };

    auto it = std::find_if(families.begin(), families.end(), sameName);
    VERIFY(it == families.end(), "Family name already exists");

    auto family = std::make_unique<Family<T>>(name, help, labels);
    auto& ref = *family;
    families.push_back(std::move(family));
    return ref;
}

template Family<Counter>& Registry::Add(const std::string& name, const std::string& help, const Labels& labels);

template Family<Gauge>& Registry::Add(const std::string& name, const std::string& help, const Labels& labels);

template Family<Histogram>& Registry::Add(const std::string& name, const std::string& help, const Labels& labels);

template <typename T>
bool Registry::Remove(const Family<T>& family) {
    std::lock_guard<std::mutex> lock{mutex_};

    auto& families = GetFamilies<T>();
    auto sameFamily = [&family](const std::unique_ptr<Family<T>>& in) { return &family == in.get(); };

    auto it = std::find_if(families.begin(), families.end(), sameFamily);
    if (it == families.end()) {
        return false;
    }

    families.erase(it);
    return true;
}

template bool Registry::Remove(const Family<Counter>& family);

template bool Registry::Remove(const Family<Gauge>& family);

template bool Registry::Remove(const Family<Histogram>& family);

} // namespace postboy::prometheus
