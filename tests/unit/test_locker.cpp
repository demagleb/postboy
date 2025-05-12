#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <common/concurrency/locker.hpp>
#include <common/system/high_resolution_clock.hpp>

using namespace std::chrono_literals;

TEST(Locker, LockUnlock) {
    static volatile int temp;
    common::concurrency::Locker locker(temp);
}

TEST(Locker, MutualExclusion) {
    static volatile int m;
    bool cs = false;

    std::thread locker([&]() {
        common::concurrency::Locker locker(m);
        cs = true;
        std::this_thread::sleep_for(3s);
        cs = false;
    });

    std::this_thread::sleep_for(1s);
    {
        common::concurrency::Locker locker(m);
        ASSERT_FALSE(cs);
    }

    locker.join();
}

// TEST(Locker, Blocking) {
//     common::system::TimeRdtsc::Init();
//     common::system::TimeRdtsc cpuTimer;
//     static volatile int m;

//     // Warmup
//     {
//         common::concurrency::Locker locker(m);
//     }

//     std::thread sleeper([&]() {
//         common::concurrency::Locker locker(m);
//         std::this_thread::sleep_for(3s);
//     });

//     std::thread waiter([&]() {
//         std::this_thread::sleep_for(1s);

//         cpuTimer.Start();
//         {
//             common::concurrency::Locker locker(m);
//         }
//         cpuTimer.Stop();

//         auto elapsed = cpuTimer.GetElapsedTime();

//         ASSERT_TRUE(elapsed < std::chrono::duration_cast<std::chrono::nanoseconds>(2000ms).count());
//     });

//     sleeper.join();
//     waiter.join();
// }

TEST(Locker, LockerTestTime) {
    common::system::TimeRdtsc::Init();
    common::system::TimeRdtsc cpuTimer;
    static volatile int m;

    // Warmup
    { common::concurrency::Locker locker(m); }

    cpuTimer.Start();
    { common::concurrency::Locker locker(m); }
    cpuTimer.Stop();

    auto elapsed = cpuTimer.GetElapsedTime();

    std::cerr << elapsed << "\n";
}
// clang-format off
