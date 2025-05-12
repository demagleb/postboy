#include "wait_group.hpp"

namespace common::concurrency {

WaitGroup::WaitGroup() = default;

void WaitGroup::Add(uint32_t count) {
    counter_.fetch_add(count);
}

void WaitGroup::Done() {
    workers_.fetch_add(1);
    if (counter_.fetch_sub(1) == 1) {
        counter_.notify_one();
    }
    workers_.fetch_sub(1);
}

void WaitGroup::Wait() {
    uint32_t old = 0;

    while ((old = counter_.load()) != 0) {
        counter_.wait(old);
    }
    counter_.notify_one();
}

WaitGroup::~WaitGroup() {
    while (workers_.load() != 0) {
        ;
    }
}

} // namespace common::concurrency
