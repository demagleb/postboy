#pragma once

#include "metric_family.hpp"

#include <sstream>

namespace postboy::prometheus {

class TextSerializer {
public:
    TextSerializer();

    std::string Serialize(const std::vector<MetricFamily>& metrics) const;

private:
    void Serialize(std::ostream& out, const std::vector<MetricFamily>& metrics) const;

private:
    mutable std::ostringstream out_;
};

} // namespace postboy::prometheus
