#pragma once

#include <config/proto/process.pb.h>
#include <postboy/base/metrics.hpp>
#include <postboy/base/queues/queue_base.hpp>
#include <postboy/base/binding.hpp>
#include <postboy/prometheus_metrics/registry.hpp>

#include <fmt/format.h>

#include <limits>
#include <cstdint>

namespace postboy {

class Processor : public IMetrics {
public:
    using Config = config::Process;

public:
    static constexpr uint64_t kInfCallNumber = std::numeric_limits<uint64_t>::max();

public:
    explicit Processor(uint64_t callNumber = kInfCallNumber)
        : IMetrics()
        , callNumber_(callNumber) {
    }

    virtual void Init(const Config& config) = 0;

    // The name is guaranteed to be unique
    // default empty
    virtual void InitMetrics(const std::string& name) {
    }

    virtual void Process() = 0;

    virtual void Finalize() {
    }

    virtual IMetrics Metrics() const;

    virtual bool IsTakeable() const;

    virtual bool IsPutable() const;

    virtual void LinkInputQueue(size_t index, std::shared_ptr<IQueueBase> queue);

    virtual void LinkOutputQueue(size_t index, std::shared_ptr<IQueueBase> queue);

    void UnlinkQueues();

    virtual std::unique_ptr<Binding> CreateOutputBinding(size_t index);

    virtual ~Processor() = default;

    bool IsValid() const;

    FORCE_INLINE uint64_t CallNumber() const {
        return callNumber_;
    }

protected:
    virtual void UnlinkInputQueues();

    virtual void UnlinkOutputQueues();

    virtual bool IsValidPutableProcessor() const;

    virtual bool IsValidTakeableProcessor() const;

    void RequestFinish();

    void RequestStop();

private:
    const uint64_t callNumber_{kInfCallNumber};
};

} // namespace postboy
