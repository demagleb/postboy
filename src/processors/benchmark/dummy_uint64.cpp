#include <common/util/assert.hpp>
#include <postboy/base/takeable_processor.hpp>
#include <postboy/base/putable_processor.hpp>
#include <postboy/base/registry.hpp>
#include <cstdint>
#include <thread>

namespace postboy {

namespace {

class DummyUInt64 : public TakeableProcessor<uint64_t>, public PutableProcessor<uint64_t> {
public:
    void Process() override {
        auto inp = Take();
        if (inp) {
            Put(std::move(*inp));
        } else {
            RequestFinish();
        }
    }

    void Init(const config::Process& config) override {
        ASSERT(config.Processes_case() == config::Process::kDummy,
               fmt::format("Process case for [DummyUInt64] from config is not [kDummyUInt64]"));
    }
};

} // namespace

REGISTER_PROCESSOR(DummyUInt64, config::Process::ProcessesCase::kDummyUInt64);

} // namespace postboy
