#pragma once

#include <common/util/types.hpp>

#include <atomic>
#include <cstdint>

namespace common::concurrency {

template <class T>
class AtomicPointers {
public:
    AtomicPointers(T* a, T* b)
        : a_(reinterpret_cast<uintptr_t>(a))
        , ab_(reinterpret_cast<uintptr_t>(a) ^ reinterpret_cast<uintptr_t>(b)) {
    }

    FORCE_INLINE T* First() {
        return reinterpret_cast<T*>(a_.load(std::memory_order_acquire));
    }

    FORCE_INLINE T* Second() {
        return reinterpret_cast<T*>(a_.load(std::memory_order_acquire) ^ ab_);
    }

    FORCE_INLINE void Exchange() {
        a_.fetch_xor(ab_, std::memory_order_release);
    }

private:
    uintptr_t ab_;
    std::atomic<uintptr_t> a_;
};

} // namespace common::concurrency
