#include "benchmark_queue.h"

#include <common/concurrency/spsc_queue.hpp>

#include <cstdint>

constexpr size_t kQueueSize = 4096;

static void Int32Closeable(benchmark::State& state) {
    common::concurrency::SpscQueue<int32_t, kQueueSize> q;
    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
        [&](size_t batchSize) {
            int i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                q.Push(++i);
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Pop();
            }
        });
}

BENCHMARK(Int32Closeable);

static void Int64Closeable(benchmark::State& state) {
    common::concurrency::SpscQueue<int64_t, kQueueSize> q;
    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
        [&](size_t batchSize) {
            int64_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                q.Push(++i);
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Pop();
            }
        });
}
BENCHMARK(Int64Closeable);


static void StringCloseable(benchmark::State& state) {
    common::concurrency::SpscQueue<std::string, kQueueSize> q;
    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Push({});
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Pop();
            }
        });
}
BENCHMARK(StringCloseable);


static void Int32NotClosable(benchmark::State& state) {
    common::concurrency::SpscQueue<int32_t, kQueueSize, false> q;
    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
        [&](size_t batchSize) {
            int i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                q.Push(++i);
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Pop();
            }
        });
}
BENCHMARK(Int32NotClosable);

static void Int64NotClosable(benchmark::State& state) {
    common::concurrency::SpscQueue<int64_t, kQueueSize, false> q;
    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
        [&](size_t batchSize) {
            int64_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                q.Push(++i);
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Pop();
            }
        });
}
BENCHMARK(Int64NotClosable);


static void StringNotClosable(benchmark::State& state) {
    common::concurrency::SpscQueue<int64_t, kQueueSize, false> q;
    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Push({});
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Pop();
            }
        });
}
BENCHMARK(StringNotClosable);

BENCHMARK_MAIN();
