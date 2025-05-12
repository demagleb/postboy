#pragma once

#include "client_metric.hpp"
#include "details/builder.hpp"
#include "metric_types.hpp"

#include <common/util/assert.hpp>
#include <common/util/types.hpp>

#include <atomic>

namespace postboy::prometheus {

namespace {

template <class ForwardIterator>
bool IsSorted(ForwardIterator first, ForwardIterator last) {
    return std::adjacent_find(first, last,
                              std::greater_equal<typename std::iterator_traits<ForwardIterator>::value_type>()) == last;
}

} // namespace

class Histogram {
public:
    using BucketBoundaries = std::vector<double>;

    explicit Histogram(const BucketBoundaries& buckets)
        : bucketBounds_(buckets)
        , bucketCounts_(buckets.size() + 1) {
        VERIFY(IsSorted(bucketBounds_.begin(), bucketBounds_.end()), "Bucket bounds in histogram is not sorted");
    }

    explicit Histogram(BucketBoundaries&& buckets)
        : bucketBounds_(buckets)
        , bucketCounts_(buckets.size() + 1) {
        VERIFY(IsSorted(bucketBounds_.begin(), bucketBounds_.end()), "Bucket bounds in histogram is not sorted");
    }

    FORCE_INLINE void Observe(double value) {
        const auto index = static_cast<std::size_t>(
            std::distance(bucketBounds_.begin(), std::lower_bound(bucketBounds_.begin(), bucketBounds_.end(), value)));

        bucketCounts_[index].fetch_add(1, std::memory_order_release);
        sum_.fetch_add(value, std::memory_order_release);
    }

    FORCE_INLINE void Reset() {
        for (int i = 0; i < static_cast<int>(bucketCounts_.size()); ++i) {
            bucketCounts_[i].store(0, std::memory_order_release);
        }
        sum_.store(0, std::memory_order_release);
    }

    FORCE_INLINE ClientMetric Collect() const {
        auto result = ClientMetric{};

        int count = 0;
        result.histogram.buckets.reserve(bucketCounts_.size());
        for (int i = 0; i < static_cast<int>(bucketCounts_.size()); ++i) {
            count += bucketCounts_[i].load(std::memory_order_acquire);

            auto bucket = ClientMetric::Bucket{};
            bucket.cumulative_count = count;
            bucket.upper_bound =
                (i == bucketBounds_.size() ? std::numeric_limits<double>::infinity() : bucketBounds_[i]);

            result.histogram.buckets.push_back(std::move(bucket));
        }

        result.histogram.sample_count = count;
        result.histogram.sample_sum = sum_.load(std::memory_order_acquire);

        return result;
    }

    static constexpr MetricType MetricType() {
        return MetricType::Histogram;
    }

private:
    const BucketBoundaries bucketBounds_;

    std::atomic<double> sum_;
    std::vector<std::atomic<double>> bucketCounts_;
};

using HistogramPtr = Histogram*;

detail::Builder<Histogram> BuildHistogram();

} // namespace postboy::prometheus
