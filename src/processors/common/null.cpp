#include <postboy/base/processor.hpp>
#include <postboy/base/putable_processor.hpp>
#include <postboy/base/registry.hpp>

#include <common/util/assert.hpp>

namespace postboy {

namespace {

class Null : public TakeableProcessor<int64_t> {
public:
    void Process() override {
        auto inp = Take();
        if (!inp) {
            RequestFinish();
        }
    }

    void Init(const config::Process& config) override {
        ENSURE(config.Processes_case() == config::Process::kNull, "Process case for [Null] from config is not [kNull]");
    }
};

} // namespace

REGISTER_PROCESSOR(Null, config::Process::ProcessesCase::kNull);

} // namespace postboy
