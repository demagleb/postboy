#include "fmt/core.h"
#include <algorithm>
#include <common/util/assert.hpp>
#include <future>
#include <postboy/base/takeable_processor.hpp>
#include <postboy/base/putable_processor.hpp>
#include <postboy/base/registry.hpp>
#include <cstdint>

namespace postboy {

namespace {

void PrintPercentiles(std::vector<uint64_t>& latencies) {
    std::sort(latencies.begin(), latencies.end());
    INFO(fmt::format("p50: {} p75: {} p99: {}", latencies[latencies.size() * 0.5], latencies[latencies.size() * 0.75],
                     latencies[latencies.size() * 0.99]));
}

class LatencyCounter : public TakeableProcessor<uint64_t> {
public:
    void Process() override {
        for (int i = 0; i < (32 << 10); ++i) {
            auto inp = Take();
            if (!inp) {
                RequestFinish();
                return;
            }
        }
        for (int i = 0; i < (1 << 10); ++i) {
            auto inp = Take();
            uint64_t now = std::chrono::duration_cast<std::chrono::nanoseconds>(
                               std::chrono::high_resolution_clock::now().time_since_epoch())
                               .count();
            if (!inp) {
                RequestFinish();
                return;
            }
            latencies_.push_back(now - *inp);
        }
        PrintPercentiles(latencies_);
        latencies_.clear();
    }

    void Init(const config::Process& config) override {
        ASSERT(config.Processes_case() == config::Process::kLatencyCounter,
               fmt::format("Process case for [LatencyCounter] from config is not [kLatencyCounter]"));
        latencies_.reserve(1 << 10);
    }

private:
    std::vector<uint64_t> latencies_;
};

} // namespace

REGISTER_PROCESSOR(LatencyCounter, config::Process::ProcessesCase::kLatencyCounter);

} // namespace postboy
