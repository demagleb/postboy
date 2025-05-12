#include "high_resolution_clock.hpp"

#include <array>
#include <atomic>
#include <cstdint>
#include <vector>

namespace common::system {

class LatencySampler {
public:
    std::vector<uint64_t> GetQuantiles(std::vector<double> quantiles) {
        std::vector<uint64_t> samples;
        samples.resize(samples_.size());
        for (size_t i = 0; i < samples_.size(); ++i) {
            samples[i] = samples_[i];
        }
        std::vector<uint64_t> res;
        res.reserve(quantiles.size());
        for (double q : quantiles) {
            if (q >= 1) {
                res.push_back(samples.back());
            } else {
                res.push_back(samples[samples.size() * q]);
            }
        }
        return res;
    }

private:
    void PutSample(uint64_t sample) {
        samples_[position_.fetch_add(1, std::memory_order_relaxed)].store(sample, std::memory_order_relaxed);
    }

    static constexpr size_t kSize = 4096;

    std::array<std::atomic<uint64_t>, kSize> samples_;
    std::atomic<size_t> position_ = 0;
};

} // namespace common::system
