#pragma once

#include <common/util/types.hpp>
#include <common/util/assert.hpp>
#include "queue_base.hpp"

#include <atomic>
#include <common/concurrency/moodycamel_readerwriter_queue.hpp>
#include <common/concurrency/spsc_queue.hpp>
#include <thread>

namespace postboy {

template <typename T>
class SPSCQueue : public IQueueBase {
public:
    explicit SPSCQueue()
        : queue_(kCapacity) {
    }

    virtual ~SPSCQueue() = default;

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

    FORCE_INLINE void IncreaseActiveProducers() {
        ENSURE(activeProducers_.fetch_add(1, std::memory_order_relaxed) == 0, "Two active producers in SPSC queue");
        ActiveProducers();
    }

    FORCE_INLINE void DecreaseActiveProducers() {
        ENSURE(activeProducers_.fetch_sub(1, std::memory_order_relaxed) == 1, "No active producers in SPSC queue");
        ActiveProducers();
    }

    FORCE_INLINE uint64_t ActiveProducers() const {
        auto res = activeProducers_.load(std::memory_order_relaxed);
        return res;
    }

    FORCE_INLINE size_t Size() const {
        return queue_.size_approx();
    }

    static constexpr size_t kCapacity = 4096;

private:
    std::atomic<uint64_t> activeProducers_{0};
    moodycamel::ReaderWriterQueue<T> queue_;
};

} // namespace postboy
