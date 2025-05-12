#include "prepared.hpp"

#include <common/util/assert.hpp>
#include <postboy/base/processor.hpp>
#include <postboy/base/registry.hpp>

#include <fmt/format.h>

namespace postboy {

namespace {

enum class PreparedItemState { NotInitialized = 0, Initialized, Run, End };

} // namespace

class Prepared::PreparedItem {
public:
    using Config = config::Postboy;

    explicit PreparedItem(const Config& config);

    FORCE_INLINE std::string_view Name() const {
        return name_;
    }

    FORCE_INLINE void Init() {
        ASSERT(state_ == PreparedItemState::NotInitialized, "Postboy already initialized");
        processor_->InitMetrics(name_);
        processor_->Init(config_);
        state_ = PreparedItemState::Initialized;
    }

    void Run();

    FORCE_INLINE PreparedItemState State() const {
        return state_;
    }

private:
    std::string name_;
    Processor::Config config_;
    std::unique_ptr<Processor> processor_;
    PreparedItemState state_{PreparedItemState::NotInitialized};
};

Prepared::PreparedItem::PreparedItem(const PreparedItem::Config& config)
    : name_(config.name())
    , config_(config.process())
    , processor_(ProcessorRegistry::Instance()->GetProcessorFactory(config_.Processes_case())()) {
    ENSURE(!processor_->IsTakeable(), fmt::format("Prepared postboy [{}] should not be takeable", Name()));
    ENSURE(!processor_->IsPutable(), fmt::format("Prepared postboy [{}] should not be putable", Name()));
    ENSURE(processor_->CallNumber() != Processor::kInfCallNumber,
           fmt::format("Prepared postboy [{}] should not be with the processor that runs an infinite number of times",
                       Name()));
}

void Prepared::PreparedItem::Run() {
    ASSERT(state_ == PreparedItemState::Initialized, "Postboy is not initialized");
    state_ = PreparedItemState::Run;
    for (size_t i = 0; i < processor_->CallNumber(); ++i) {
        processor_->Process();
    }
    state_ = PreparedItemState::End;
}

Prepared::Prepared(const Prepared::Config& config)
    : preparedItems_(CollectItems(config)) {
}

void Prepared::Run() {
    for (auto& p : preparedItems_) {
        p->Init();
        p->Run();
        p.reset();
    }
    preparedItems_.clear();
}

std::vector<std::unique_ptr<Prepared::PreparedItem>> Prepared::CollectItems(const Prepared::Config& config) {
    std::vector<std::unique_ptr<Prepared::PreparedItem>> result;

    for (const auto& c : config) {
        result.emplace_back(std::make_unique<PreparedItem>(c));
    }

    return result;
}

Prepared::~Prepared() = default;

} // namespace postboy
