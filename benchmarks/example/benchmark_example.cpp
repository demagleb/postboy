#include <benchmark/benchmark.h>
#include <common/system/high_resolution_clock.hpp>

#include <string>
#include <iostream>
#include <chrono>

static void TimeRdtsc(benchmark::State& state) {
    uint64_t time = 0;
    common::system::TimeRdtsc::Init();
    while (state.KeepRunningBatch(128)) {
        for (size_t i = 0; i < 128; ++i) {
            time = common::system::TimeRdtsc::GetCurrentTime();
            benchmark::DoNotOptimize(time);
        }
    }
}
BENCHMARK(TimeRdtsc);

static void ChronoHighResolutionClock(benchmark::State& state) {
    auto time = std::chrono::high_resolution_clock::now();
    while (state.KeepRunningBatch(128)) {
        for (size_t i = 0; i < 128; ++i) {
            time = std::chrono::high_resolution_clock::now();
            benchmark::DoNotOptimize(time);
        }
    }
}
BENCHMARK(ChronoHighResolutionClock);

BENCHMARK_MAIN();
