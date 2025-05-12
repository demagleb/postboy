#pragma once

#include <atomic>

namespace common::concurrency {

struct Spinlock {
    std::atomic<bool> lock_ = {false};

    void lock() noexcept { // NOLINT
        for (;;) {
            if (!lock_.exchange(true, std::memory_order_acquire)) {
                return;
            }
            while (lock_.load(std::memory_order_relaxed)) {
                __builtin_ia32_pause();
            }
        }
    }

    bool try_lock() noexcept { // NOLINT
        return !lock_.load(std::memory_order_relaxed) && !lock_.exchange(true, std::memory_order_acquire);
    }

    void unlock() noexcept { // NOLINT
        lock_.store(false, std::memory_order_release);
    }

    void Unlock() noexcept {
        unlock();
    }

    void Lock() noexcept {
        lock();
    }

    void TryLock() noexcept {
        try_lock();
    }
};

} // namespace common::concurrency
