#pragma once

#include <atomic>
#include <common/util/assert.hpp>
#include <common/util/types.hpp>
#include <config/proto/postboy.pb.h>
#include <postboy/base/metrics.hpp>
#include <postboy/base/processor.hpp>
#include <postboy/base/registry.hpp>
#include <thread_manager/thread_manager.hpp>

#include <memory>
#include <thread>
#include <array>

namespace postboy {

enum class PostboyState : uint8_t {
    NotInitialized = 0,
    Initialized = 1,
    Running = 2,
    StopRequested = 3,
    Stopped = 4,
    ResumeRequested = 5,
    FinishRequested = 6,
    Finished = 7,
    SampleRequested = 8,
    Sampling = 9,
};

class Postboy {
public:
    explicit Postboy(config::Postboy config);

    FORCE_INLINE const std::string& Name() const {
        return config_.name();
    }

    FORCE_INLINE const config::Postboy& Config() const {
        return config_;
    }

    FORCE_INLINE PostboyState State() const {
        return state_.load(std::memory_order_relaxed);
    }

    FORCE_INLINE bool IsTakeable() const {
        return processor_->IsTakeable();
    }

    FORCE_INLINE bool IsPutable() const {
        return processor_->IsPutable();
    }

    FORCE_INLINE bool IsValid() const {
        return processor_->IsValid();
    }

    FORCE_INLINE postboy::Processor& Processor() const {
        return *processor_;
    }

    void Init();

    void Run();

    void Wait(PostboyState& currentState);

    void RequestStop();

    void RequestResume();

    void RequestFinish();

    void RequestSample(std::chrono::nanoseconds duration);

    std::vector<uint64_t> GetLatencies() const;

    static Postboy& Current();

private:
    void RunImpl();

private:
    const config::Postboy config_;
    size_t processedCalls_ = 0;
    std::unique_ptr<postboy::Processor> processor_;
    std::optional<std::jthread> thread_;
    std::atomic<PostboyState> state_{PostboyState::NotInitialized};

    static constexpr size_t kSampleDurationsSize = 4096;
    std::array<std::atomic<uint64_t>, kSampleDurationsSize> sampleDurations_;
    std::atomic<uint64_t> sampleEnd_ = 0;
    size_t sampleDurationsIndex_ = 0;
};

} // namespace postboy
