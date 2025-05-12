#include <common/util/assert.hpp>
#include <postboy/base/putable_processor.hpp>
#include <postboy/base/registry.hpp>
#include <cstdint>
#include <chrono>
#include <thread>

namespace postboy {

namespace {

class Now : public PutableProcessor<uint64_t> {
public:
    void Process() override {
        Put(std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch())
                .count());
        std::this_thread::sleep_for(std::chrono::nanoseconds(20));
    }

    void Init(const config::Process& config) override {
        ASSERT(config.Processes_case() == config::Process::kNow,
               fmt::format("Process case for [Now] from config is not [kNow]"));
    }
};

} // namespace

REGISTER_PROCESSOR(Now, config::Process::ProcessesCase::kNow);

} // namespace postboy
