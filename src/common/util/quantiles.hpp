#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

namespace common::util {

template <typename T>
struct Quantile {
    int idx;
    T value;
};

template <typename T>
std::vector<Quantile<T>> Calculate(typename std::vector<T>::const_iterator first,
                                   typename std::vector<T>::const_iterator last, const std::vector<int>& quantilesIdx) {
    std::vector<Quantile<T>> result;

    const uint64_t valuesCount = last - first;
    if (valuesCount < quantilesIdx.size()) {
        return result;
    }

    result.reserve(quantilesIdx.size());

    for (const auto& quantile : quantilesIdx) {
        uint64_t index = valuesCount * quantile / 100;
        if (index) {
            --index;
        }
        result.push_back({quantile, *(first + index)});
    }

    return result;
}

template <class T>
std::ostream& Print(std::ostream& s, const std::vector<Quantile<T>>& quantiles) {
    for (size_t i = 0; i < quantiles.size(); ++i) {
        const auto& data = quantiles[i];
        s << data.value << " ";
    }
    s << std::endl;
    return s;
}

template <class T>
std::ostream& PrintWithIdx(std::ostream& s, const std::vector<Quantile<T>>& quantiles) {
    for (size_t i = 0; i < quantiles.size(); ++i) {
        const auto& data = quantiles[i];
        s << data.value << "," << data.idx << " ";
    }
    s << std::endl;
    return s;
}

} // namespace common::util
