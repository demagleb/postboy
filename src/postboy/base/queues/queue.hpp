#pragma once

#include "spsc.hpp"
#include "mpmc.hpp"
#include "queue_base.hpp"

#include <common/util/types.hpp>

#include <stdexcept>
#include <utility>
#include <memory>
#include <variant>

namespace postboy {

template <typename T>
class BoxedProducerQueue {
public:
    BoxedProducerQueue() = default;

    void SetImpl(std::shared_ptr<IQueueBase> impl) {
        Clear();
        if (auto queue = dynamic_cast<SPSCQueue<T>*>(impl.get())) {
            queue_ = queue;
            queue->IncreaseActiveProducers();
            impl_ = std::move(impl);
            return;
        }
        if (auto queue = dynamic_cast<MPMCQueue<T>*>(impl.get())) {
            queue_ = queue;
            queue->IncreaseActiveProducers();
            impl_ = std::move(impl);
            return;
        }
        throw std::invalid_argument("Invalid queue type");
    }

    BoxedProducerQueue(const BoxedProducerQueue&) = delete;

    BoxedProducerQueue(BoxedProducerQueue&&) = delete;

    ~BoxedProducerQueue() {
        Clear();
    }

    FORCE_INLINE void Put(T elem) {
        std::visit([&](auto* queue) { queue->Put(std::move(elem)); }, queue_);
    }

    FORCE_INLINE void Clear() {
        if (impl_) {
            std::visit([](auto* queue) { queue->DecreaseActiveProducers(); }, queue_);
        }
        impl_.reset();
        queue_ = {};
    }

    FORCE_INLINE bool Valid() const {
        return impl_ != nullptr;
    }

private:
    std::variant<SPSCQueue<T>*, MPMCQueue<T>*> queue_;
    std::shared_ptr<IQueueBase> impl_;
};

template <typename T>
class BoxedConsumerQueue {
public:
    BoxedConsumerQueue() = default;

    void SetImpl(std::shared_ptr<IQueueBase> impl) {
        Clear();
        if (auto queue = dynamic_cast<SPSCQueue<T>*>(impl.get())) {
            queue_ = queue;
            impl_ = std::move(impl);
            return;
        }
        if (auto queue = dynamic_cast<MPMCQueue<T>*>(impl.get())) {
            queue_ = queue;
            impl_ = std::move(impl);
            return;
        }
        throw std::invalid_argument("Invalid queue type");
    }

    BoxedConsumerQueue(const BoxedConsumerQueue&) = delete;

    BoxedConsumerQueue(BoxedConsumerQueue&&) = delete;

    FORCE_INLINE std::optional<T> Take() {
        return std::visit([&](auto* queue) { return queue->Take(); }, queue_);
    }

    FORCE_INLINE void Clear() {
        queue_ = {};
        impl_.reset();
    }

    FORCE_INLINE bool Valid() const {
        return impl_ != nullptr;
    }

private:
    std::variant<SPSCQueue<T>*, MPMCQueue<T>*> queue_;
    std::shared_ptr<IQueueBase> impl_;
};

} // namespace postboy
