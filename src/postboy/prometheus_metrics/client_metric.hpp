#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

namespace postboy::prometheus {

struct ClientMetric {
    // Label
    struct Label {
        std::string name;
        std::string value;

        bool operator<(const Label& rhs) const {
            return std::tie(name, value) < std::tie(rhs.name, rhs.value);
        }

        bool operator==(const Label& rhs) const {
            return std::tie(name, value) == std::tie(rhs.name, rhs.value);
        }
    };
    std::vector<Label> label;

    // Counter
    struct Counter {
        double value = 0.0;
    };
    Counter counter;

    // Gauge
    struct Gauge {
        double value = 0.0;
    };
    Gauge gauge;

    // Histogram
    struct Bucket {
        int cumulative_count = 0;
        double upper_bound = 0.0;
    };

    struct Histogram {
        int sample_count = 0;
        double sample_sum = 0.0;
        std::vector<Bucket> buckets;
    };
    Histogram histogram;

    std::int64_t timestamp_ms = 0;
};

} // namespace postboy::prometheus
