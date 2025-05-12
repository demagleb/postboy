#include "processor.hpp"
#include "postboy/base/metrics.hpp"
#include "postboy/base/postboy.hpp"

namespace postboy {

IMetrics Processor::Metrics() const {
    return IMetrics({});
}

bool Processor::IsTakeable() const {
    return false;
}

bool Processor::IsPutable() const {
    return false;
}

void Processor::LinkInputQueue(size_t index, std::shared_ptr<IQueueBase> queue) {
    throw std::runtime_error("Can not link input queue");
}

void Processor::LinkOutputQueue(size_t index, std::shared_ptr<IQueueBase> queue) {
    throw std::runtime_error("Can not link output queue");
}

void Processor::UnlinkQueues() {
    if (IsTakeable()) {
        UnlinkInputQueues();
    }
    if (IsValidPutableProcessor()) {
        UnlinkOutputQueues();
    }
}

std::unique_ptr<Binding> Processor::CreateOutputBinding(size_t index) {
    throw std::runtime_error("Can not create input queue");
}

void Processor::UnlinkInputQueues() {
    throw std::runtime_error("Can not unlink input queues");
}

void Processor::UnlinkOutputQueues() {
    throw std::runtime_error("Can not unlink output queue");
}

bool Processor::IsValidPutableProcessor() const {
    return false;
}

bool Processor::IsValidTakeableProcessor() const {
    return false;
}

bool Processor::IsValid() const {
    return (!IsTakeable() || IsValidTakeableProcessor()) && (!IsPutable() || IsValidPutableProcessor());
}

void Processor::RequestFinish() {
    Postboy::Current().RequestFinish();
}

void Processor::RequestStop() {
    Postboy::Current().RequestStop();
}

} // namespace postboy
