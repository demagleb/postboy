#include <gtest/gtest.h>

#include <common/system/high_resolution_clock.hpp>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

TEST(HighResolutionClock, Experiment) {
    common::system::TimeRdtsc::Init();

    auto timer = common::system::TimeRdtsc();

    timer.Start();
    std::this_thread::sleep_for(1s);
    timer.Stop();

    std::cout << timer.GetElapsedTime() << std::endl;
}
