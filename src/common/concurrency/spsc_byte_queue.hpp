#pragma once

#include <common/util/types.hpp>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <memory>

namespace common::concurrency {

class SpscByteQueue {
public:
    using Ptr = std::unique_ptr<SpscByteQueue>;
    using SharedPtr = std::shared_ptr<SpscByteQueue>;

public:
    explicit SpscByteQueue(int64_t size, int64_t cellSize) {
        begin_ = reinterpret_cast<char*>(::calloc(size, sizeof(char)));
        head_ = reinterpret_cast<char*>(begin_);
        tail_ = reinterpret_cast<char*>(begin_);
        size_ = size;
        maxCount_ = size_ / cellSize;
    }

    inline int MaxCount() const {
        return maxCount_;
    }

    ~SpscByteQueue() {
        ::free(begin_);
    }

    FORCE_INLINE bool Empty() {
        return count_.load(std::memory_order_acquire) == 0;
    }

    FORCE_INLINE bool Available() {
        return count_.load(std::memory_order_acquire) != maxCount_;
    }

    FORCE_INLINE void Store(const char* bytes, size_t size) {
        while (UNLIKELY(!Available())) {
            ::_mm_pause();
        }
        StoreImpl(bytes, size);
    }

    FORCE_INLINE bool TryStore(const char* bytes, size_t size) {
        if (UNLIKELY(!Available())) {
            return false;
        }
        StoreImpl(bytes, size);
        return true;
    }

    FORCE_INLINE void Load(char* bytes, size_t size) {
        while (UNLIKELY(Empty())) {
            _mm_pause();
        }
        LoadImpl(bytes, size);
    }

    FORCE_INLINE bool TryLoad(char* bytes, size_t size) {
        if (UNLIKELY(Empty())) {
            return false;
        }
        LoadImpl(bytes, size);
        return true;
    }

private:
    FORCE_INLINE void StoreImpl(const char* bytes, size_t size) {
        ::memcpy(tail_, bytes, size);
        tail_ += size;
        if (UNLIKELY(tail_ - begin_ == size_)) {
            tail_ = begin_;
        }
        count_.fetch_add(1, std::memory_order_acq_rel);
    }

    FORCE_INLINE void LoadImpl(char* bytes, size_t size) {
        ::memcpy(bytes, head_, size);
        head_ += size;
        if (UNLIKELY(head_ - begin_ == size_)) {
            head_ = begin_;
        }
        count_.fetch_sub(1, std::memory_order_acq_rel);
    }

private:
    alignas(64) char* begin_{nullptr};
    alignas(64) char* head_{nullptr};    // only use by consumer to read
    alignas(64) char* tail_{nullptr};    // only use by producer to store
    alignas(64) std::atomic<int> count_; // current elements count in queue
    alignas(64) volatile int maxCount_;  // maximum elements that can be in queue
    alignas(64) volatile int64_t size_;  // size of the queue
};

} // namespace common::concurrency
