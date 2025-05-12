#include <benchmark/benchmark.h>

#include <common/concurrency/spsc_byte_queue.hpp>

#include <cstdint>
#include <thread>

constexpr size_t kBatchSize = 1000;

static void Int32(benchmark::State& state) {
    common::concurrency::SpscByteQueue q(100 * sizeof(int32_t), sizeof(int32_t));
    std::atomic<bool> shouldStop{false};

    std::thread t1([&]() {
        int32_t i = 0;
        while (state.KeepRunningBatch(kBatchSize)) {
            for (int j = 0; j < kBatchSize; ++j) {
                q.Store(reinterpret_cast<const char*>(&i), sizeof(i));
            }
        }
        for (int j = 0; j < kBatchSize - 1; ++j) {
            q.Store(reinterpret_cast<const char*>(&i), sizeof(i));
        }
        shouldStop.store(true);
        q.Store(reinterpret_cast<const char*>(&i), sizeof(i));
    });

    std::thread t2([&]() {
        int32_t i = 0;
        while (!shouldStop.load()) {
            for (int j = 0; j < kBatchSize; ++j) {
                q.Load(reinterpret_cast<char*>(&i), sizeof(i));
            }
        }
    });

    t1.join();
    t2.join();
}
BENCHMARK(Int32);

static void Int64(benchmark::State& state) {
    common::concurrency::SpscByteQueue q(100 * sizeof(int64_t), sizeof(int64_t));
    std::atomic<bool> shouldStop{false};

    std::thread t1([&]() {
        int64_t i = 0;
        while (state.KeepRunningBatch(kBatchSize)) {
            for (int j = 0; j < kBatchSize; ++j) {
                q.Store(reinterpret_cast<const char*>(&i), sizeof(i));
            }
        }
        for (int j = 0; j < kBatchSize - 1; ++j) {
            q.Store(reinterpret_cast<const char*>(&i), sizeof(i));
        }
        shouldStop.store(true);
        q.Store(reinterpret_cast<const char*>(&i), sizeof(i));
    });

    std::thread t2([&]() {
        int64_t i = 0;
        while (!shouldStop.load()) {
            for (int j = 0; j < kBatchSize; ++j) {
                q.Load(reinterpret_cast<char*>(&i), sizeof(i));
            }
        }
    });

    t1.join();
    t2.join();
}
BENCHMARK(Int64);

BENCHMARK_MAIN();
