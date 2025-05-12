#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <common/concurrency/spinlock.hpp>
#include <common/system/high_resolution_clock.hpp>

using namespace std::chrono_literals;

TEST(Spinlock, LockUnlock) {
    common::concurrency::Spinlock m;
    m.Lock();
    m.Unlock();
}

TEST(Spinlock, MutualExclusion) {
    bool cs = false;
    common::concurrency::Spinlock m;

    std::thread locker([&]() {
        m.Lock();
        cs = true;
        std::this_thread::sleep_for(3s);
        cs = false;
        m.Unlock();
    });

    std::this_thread::sleep_for(1s);
    m.Lock();
    ASSERT_FALSE(cs);
    m.Unlock();

    locker.join();
}

TEST(Spinlock, LockerTestTime) {
    common::system::TimeRdtsc::Init();
    common::system::TimeRdtsc cpuTimer;
    common::concurrency::Spinlock m;

    // Warmup
    m.Lock();
    m.Unlock();

    cpuTimer.Start();
    m.Lock();
    m.Unlock();
    cpuTimer.Stop();

    auto elapsed = cpuTimer.GetElapsedTime();

    std::cerr << elapsed << "\n";
}
// clang-format off
