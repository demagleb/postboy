#pragma once

#include <atomic>
#include <cstdint>

namespace common::concurrency {

class WaitGroup {
public:
    WaitGroup();

    void Add(uint32_t count);

    void Done();

    void Wait();

    ~WaitGroup();

private:
    std::atomic<uint32_t> counter_{0};
    std::atomic<size_t> workers_{0};
};

} // namespace common::concurrency
