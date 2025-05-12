#pragma once

#include <vector>

namespace postboy::prometheus {

struct MetricFamily;

class Collectable {
public:
    virtual ~Collectable() = default;

    virtual std::vector<MetricFamily> Collect() const = 0;
};

} // namespace postboy::prometheus
