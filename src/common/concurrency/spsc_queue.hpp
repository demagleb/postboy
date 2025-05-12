#pragma once

#include <common/util/types.hpp>
#include <cstddef>
#include <refer/manual.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <emmintrin.h>
#include <optional>
#include <utility>

namespace common::concurrency {

template <class T, size_t Capacity, bool Closeable = true>
class SpscQueue {
private:
    struct Slot {
        template <typename... Args>
        FORCE_INLINE void Create(Args&&... args) {
            new (static_cast<void*>(storage_)) T(std::forward<Args>(args)...);
        }

        FORCE_INLINE void Destroy() {
            GetValue().~T();
        }

        FORCE_INLINE T& GetValue() {
            return *reinterpret_cast<T*>(&storage_);
        }

    private:
        alignas(T) std::byte storage_[sizeof(T)];
    };
    static_assert(sizeof(Slot) == sizeof(T));
    static_assert(alignof(Slot) == alignof(T));

    alignas(64) size_t consumerStart_{0};
    alignas(32) size_t consumerEnd_{0};

    alignas(64) std::atomic<bool> closedFlag_ = false;

    alignas(64) size_t producerEnd_{0};
    alignas(32) size_t producerStart_{0};

    alignas(64) std::atomic<size_t> start_{0};
    alignas(64) std::atomic<size_t> end_{0};
    alignas(64) std::array<Slot, Capacity> buffer_;

public:
    ~SpscQueue() {
        auto start = start_.load(std::memory_order_acquire);
        auto end = end_.load(std::memory_order_acquire);

        for (size_t i = start; i < end; ++i) {
            buffer_[i].Destroy();
        }
    }

    FORCE_INLINE bool TryPush(T value) {
        if (producerEnd_ - producerStart_ == Capacity) {
            producerStart_ = start_.load(std::memory_order_acquire);
        }
        if (producerEnd_ - producerStart_ == Capacity || IsClosedInternal()) {
            return false;
        }
        buffer_[producerEnd_ % Capacity].Create(std::move(value));
        ++producerEnd_;
        end_.fetch_add(1, std::memory_order_release);
        return true;
    }

    FORCE_INLINE bool Push(T value) {
        if (producerEnd_ - producerStart_ == Capacity) {
            producerStart_ = start_.load(std::memory_order_acquire);
        }
        while (producerEnd_ - producerStart_ == Capacity && !IsClosedInternal()) {
            _mm_pause();
            producerStart_ = start_.load(std::memory_order_acquire);
        }
        if (producerEnd_ - producerStart_ == Capacity) {
            return false;
        }
        buffer_[producerEnd_ % Capacity].Create(std::move(value));
        ++producerEnd_;
        end_.fetch_add(1, std::memory_order_release);
        return true;
    }

    FORCE_INLINE std::optional<T> TryPop() {
        if (consumerEnd_ == consumerStart_) {
            consumerEnd_ = end_.load(std::memory_order_acquire);
        }
        if (consumerEnd_ == consumerStart_) {
            return std::nullopt;
        }
        auto res = std::move(buffer_[consumerStart_ % Capacity].GetValue());
        buffer_[consumerStart_ % Capacity].Destroy();
        ++consumerStart_;
        start_.fetch_add(1, std::memory_order_release);
        return res;
    }

    FORCE_INLINE std::optional<T> Pop() {
        if (consumerEnd_ == consumerStart_) {
            consumerEnd_ = end_.load(std::memory_order_acquire);
        }
        while (consumerEnd_ == consumerStart_ && !IsClosedInternal()) {
            _mm_pause();
            consumerEnd_ = end_.load(std::memory_order_acquire);
        }
        if (consumerEnd_ == consumerStart_) {
            return std::nullopt;
        }
        auto res = std::move(buffer_[consumerStart_ % Capacity].GetValue());
        buffer_[consumerStart_ % Capacity].Destroy();
        ++consumerStart_;
        start_.fetch_add(1, std::memory_order_release);
        return res;
    }

    FORCE_INLINE bool IsClosed() const
        requires(Closeable)
    {
        return IsClosedInternal();
    }

    // Only for producer or consumer use
    FORCE_INLINE void Close()
        requires(Closeable)
    {
        closedFlag_.store(true, std::memory_order_relaxed);
    }

private:
    FORCE_INLINE bool IsClosedInternal() const {
        if constexpr (Closeable) {
            return closedFlag_.load(std::memory_order_relaxed);
        } else {
            return false;
        }
    }
};

} // namespace common::concurrency
