#pragma once

#include "queue_base.hpp"

#include <atomic>
#include <common/util/types.hpp>
#include <common/util/panic.hpp>
#include <common/concurrency/moodycamel_concurrent_queue.hpp>

#include <cstdint>
#include <emmintrin.h>
#include <mutex>
#include <optional>
#include <iostream>
#include <thread>

namespace postboy {

template <typename T>
class MPMCQueue : public IQueueBase {
public:
    explicit MPMCQueue()
        : queue_(kCapacity) {
    }

    virtual ~MPMCQueue() = default;

    FORCE_INLINE void Put(T elem) {
        while (!queue_.try_enqueue(std::move(elem))) {
            _mm_pause();
        }
    }

    FORCE_INLINE std::optional<T> Take() {
        T value;
        while (!queue_.try_dequeue(value)) {
            if (ActiveProducers() == 0) {
                return std::nullopt;
            }
            _mm_pause();
        }
        return value;
    }

    FORCE_INLINE uint64_t ActiveProducers() const {
        return activeProducers_.load(std::memory_order_relaxed);
    }

    FORCE_INLINE void IncreaseActiveProducers() {
        activeProducers_.fetch_add(1, std::memory_order_relaxed);
    }

    FORCE_INLINE void DecreaseActiveProducers() {
        activeProducers_.fetch_sub(1, std::memory_order_relaxed);
    }

    static constexpr size_t kCapacity = 4096;

private:
    std::atomic<uint64_t> activeProducers_{0};
    moodycamel::ConcurrentQueue<T> queue_;
};

} // namespace postboy
