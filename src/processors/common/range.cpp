#include "process.pb.h"
#include "processors/common.pb.h"
#include <cstdint>
#include <optional>
#include <postboy/base/processor.hpp>
#include <postboy/base/putable_processor.hpp>
#include <postboy/base/registry.hpp>

#include <common/util/assert.hpp>

namespace postboy {

namespace {

class Range : public PutableProcessor<int64_t> {
public:
    void Process() override {
        if (r_ && i_ >= *r_) {
            RequestFinish();
            return;
        }
        Put(i_++);
    }

    void Init(const config::Process& config) override {
        ENSURE(config.Processes_case() == config::Process::kRange,
               "Process case for [Range] from config is not [kRange]");
        i_ = config.range().l();
        r_ = config.range().has_r() ? config.range().r() : std::optional<uint64_t>{};
    }

private:
    int64_t i_ = 0;
    std::optional<uint64_t> r_;
};

} // namespace

REGISTER_PROCESSOR(Range, config::Process::ProcessesCase::kRange);

} // namespace postboy
