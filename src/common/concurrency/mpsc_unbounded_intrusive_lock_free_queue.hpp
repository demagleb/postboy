#pragma once

#include <common/concurrency/mpsc_unbounded_intrusive_lock_free_stack.hpp>

#include <common/intrusive/forward_list.hpp>

namespace common::concurrency {

template <typename T>
using MPSCUnboundedIntrusiveLockFreeQueueNode = MPSCUnboundedIntrusiveLockFreeStackNode<T>;

template <typename T>
class MPSCUnboundedIntrusiveLockFreeQueue {
public:
    void Put(T* value) {
        stack_.Push(value);
    }

    T* TryTake() {
        if (acq_.IsEmpty()) {
            auto acq = stack_.TryPopAllReversed();
            acq_.Append(acq);
        }

        return acq_.PopFront();
    }

private:
    MPSCUnboundedIntrusiveLockFreeStack<T> stack_{};
    intrusive::IntrusiveForwardList<T> acq_{};
};

} // namespace common::concurrency
