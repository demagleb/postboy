#pragma once

namespace common::concurrency {

class Locker {
public:
    explicit Locker(volatile int& obj);
    ~Locker();

private:
    volatile int* obj_;
};

} // namespace common::concurrency
