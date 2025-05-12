#include "locker.hpp"

namespace common::concurrency {

Locker::Locker(volatile int& obj)
    : obj_(&obj) {
    while (__sync_lock_test_and_set(obj_, 1) != 0) {
        ;
    }
}

Locker::~Locker() {
    __sync_lock_release(obj_);
}

} // namespace common::concurrency
