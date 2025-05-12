#include <common/util/assert.hpp>
#include <postboy/base/takeable_processor.hpp>
#include <postboy/base/putable_processor.hpp>
#include <postboy/base/registry.hpp>
#include <thread>

namespace postboy {

namespace {

class Dummy : public TakeableProcessor<int64_t>, public PutableProcessor<int64_t> {
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
               fmt::format("Process case for [Dummy] from config is not [kDummy]"));
    }
};

} // namespace

REGISTER_PROCESSOR(Dummy, config::Process::ProcessesCase::kDummy);

} // namespace postboy
