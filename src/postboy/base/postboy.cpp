#include "postboy.hpp"
#include "common/util/assert.hpp"
#include "common/util/panic.hpp"
#include <atomic>
#include <cstdint>

namespace postboy {

namespace {

static thread_local Postboy* currentPostboy = nullptr;

} // namespace

Postboy::Postboy(config::Postboy config)
    : config_(std::move(config))
    , processor_(ProcessorRegistry::Instance()->GetProcessorFactory(config_.process().Processes_case())()) {
}

void Postboy::Init() {
    ASSERT(state_ == PostboyState::NotInitialized, "Postboy already initialized");
    processor_->InitMetrics(Name());
    processor_->Init(config_.process());
    state_ = PostboyState::Initialized;
}

void Postboy::Run() {
    ENSURE(state_.load(std::memory_order_relaxed) == PostboyState::Initialized, "Postboy is not initialized");
    ENSURE(!thread_, "Thread already exists");
    state_.store(PostboyState::Running, std::memory_order_relaxed);
    thread_.emplace([this]() {
        currentPostboy = this;
        RunImpl();
    });
}

void Postboy::RunImpl() {
    thread_manager::Get().Configure(Name());
    const auto callNumber = processor_->CallNumber();

    PostboyState state = State();
    while (processedCalls_ < callNumber && state != PostboyState::FinishRequested) {
        while (state == PostboyState::Running && processedCalls_ < callNumber) {
            processor_->Process();
            ++processedCalls_;
            state = State();
        }

        while (state == PostboyState::Sampling && processedCalls_ < callNumber) {
            auto start = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             std::chrono::high_resolution_clock::now().time_since_epoch())
                             .count();
            processor_->Process();
            auto end = std::chrono::duration_cast<std::chrono::nanoseconds>(
                           std::chrono::high_resolution_clock::now().time_since_epoch())
                           .count();

            sampleDurations_[sampleDurationsIndex_].store(end - start, std::memory_order_relaxed);
            sampleDurationsIndex_ = (sampleDurationsIndex_ + 1) % kSampleDurationsSize;
            ++processedCalls_;
            state = State();
            if (state == PostboyState::Sampling && end > sampleEnd_.load(std::memory_order_relaxed)) {
                state_.compare_exchange_strong(state, PostboyState::Running, std::memory_order_seq_cst);
                state_.notify_all();
            }
        }
        if (state == PostboyState::SampleRequested) {
            INFO("Postboy \"" << Name() << "\": SampleRequested");
            state_.compare_exchange_strong(state, PostboyState::Sampling, std::memory_order_seq_cst);
            state_.notify_all();
        }
        if (state == PostboyState::StopRequested) {
            INFO("Postboy \"" << Name() << "\": StopRequested");
            state_.compare_exchange_strong(state, PostboyState::Stopped, std::memory_order_seq_cst);
            state_.notify_all();
        }
        if (state == PostboyState::ResumeRequested) {
            INFO("Postboy \"" << Name() << "\": ResumeRequested");
            state_.compare_exchange_strong(state, PostboyState::Running, std::memory_order_seq_cst);
            state_.notify_all();
        }
        if (state == PostboyState::Stopped) {
            INFO("Postboy \"" << Name() << "\": Stopped");
            state_.wait(state, std::memory_order_seq_cst);
            state = State();
        }
    }
    std::atomic_thread_fence(std::memory_order_seq_cst);
    if (state == PostboyState::FinishRequested) {
        INFO("Postboy \"" << Name() << "\": FinishRequested");
    }

    processor_->Finalize();
    processor_->UnlinkQueues();
    INFO("Postboy \"" << Name() << "\""
                      << " finished");

    state_.store(PostboyState::Finished, std::memory_order_seq_cst);
    state_.notify_all();
}

void Postboy::RequestStop() {
    auto state = state_.load(std::memory_order_seq_cst);
    while (state == PostboyState::ResumeRequested || state == PostboyState::Running) {
        state_.compare_exchange_strong(state, PostboyState::StopRequested, std::memory_order_seq_cst);
        state_.notify_all();
    }
}

void Postboy::RequestResume() {
    auto state = state_.load(std::memory_order_seq_cst);
    while (state == PostboyState::StopRequested || state == PostboyState::Stopped) {
        state_.compare_exchange_strong(state, PostboyState::ResumeRequested, std::memory_order_seq_cst);
        state_.notify_all();
    }
}

void Postboy::RequestFinish() {
    auto state = state_.load(std::memory_order_seq_cst);
    while (state == PostboyState::ResumeRequested || state == PostboyState::Running ||
           state == PostboyState::StopRequested || state == PostboyState::Stopped) {
        state_.compare_exchange_strong(state, PostboyState::FinishRequested, std::memory_order_seq_cst);
        state_.notify_all();
    }
}

void Postboy::RequestSample(std::chrono::nanoseconds duration) {
    auto state = state_.load(std::memory_order_seq_cst);
    while (state == PostboyState::ResumeRequested || state == PostboyState::Running) {
        sampleEnd_.store(std::chrono::high_resolution_clock::now().time_since_epoch().count() + duration.count(),
                         std::memory_order_seq_cst);
        state_.compare_exchange_strong(state, PostboyState::SampleRequested, std::memory_order_seq_cst);
        state_.notify_all();
    }
}

void Postboy::Wait(PostboyState& currentState) {
    auto state = state_.load(std::memory_order_seq_cst);
    while (state == currentState) {
        state_.wait(state, std::memory_order_seq_cst);
        state = state_.load(std::memory_order_seq_cst);
    }
    currentState = state;
}

Postboy& Postboy::Current() {
    ENSURE(currentPostboy != nullptr, "Empty current Postboy");
    return *currentPostboy;
}

std::vector<uint64_t> Postboy::GetLatencies() const {
    std::vector<uint64_t> latencies;
    for (size_t i = 0; i < kSampleDurationsSize; ++i) {
        auto latency = sampleDurations_[i].load(std::memory_order_relaxed);
        if (latency != 0) {
            latencies.push_back(latency);
        }
    }
    return latencies;
}

} // namespace postboy
