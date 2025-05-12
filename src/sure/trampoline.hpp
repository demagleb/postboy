#pragma once

namespace sure {

// All runnable entities (coroutines, fibers, generators)
// should implement ITrampoline

struct ITrampoline {
    virtual ~ITrampoline() = default;

    // Never returns
    virtual void Run() noexcept = 0;
};

} // namespace sure
