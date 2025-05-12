#include "benchmark_queue.h"

#include <chrono>
#include <postboy/base/queues/queue.hpp>
#include <postboy/base/queues/spsc.hpp>

#include <cstdint>
#include <thread>

static void Int32(benchmark::State& state) {
    postboy::SPSCQueue<int32_t> q;
    BenchmarkQueue(
        state,
        1,
        1,
        2048,
        [&](size_t batchSize) {
            int32_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                q.Put(++i);
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Take();
            }
        });
}
BENCHMARK(Int32)->Repetitions(100000)->Iterations(2048)->ReportAggregatesOnly(true);

static void Int64(benchmark::State& state) {
    postboy::SPSCQueue<int64_t> q;
    BenchmarkQueue(
        state,
        1,
        1,
        2048,
        [&](size_t batchSize) {
            int64_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                q.Put(++i);
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Take();
            }
        });
}
BENCHMARK(Int64)->Repetitions(100000)->Iterations(2048)->ReportAggregatesOnly(true);

static void String(benchmark::State& state) {
    postboy::SPSCQueue<std::string> q;
    BenchmarkQueue(
        state,
        1,
        1,
        2048,
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Put({});
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.Take();
            }
        });
}
BENCHMARK(String)->Repetitions(100000)->Iterations(2048)->ReportAggregatesOnly(true);

static void Int32Virtual(benchmark::State& state) {
    auto qq = std::make_shared<postboy::SPSCQueue<int32_t>>();
    postboy::BoxedProducerQueue<int32_t> wq;
    wq.SetImpl(qq);
    postboy::BoxedConsumerQueue<int32_t> rq;
    rq.SetImpl(qq);
    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
        [&](size_t batchSize) {
            int32_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                wq.Put(i);
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                rq.Take();
            }
        });
}
BENCHMARK(Int32Virtual);

static void Int64Virtual(benchmark::State& state) {
    auto qq = std::make_shared<postboy::SPSCQueue<int64_t>>();
    postboy::BoxedProducerQueue<int64_t> wq;
    wq.SetImpl(qq);
    postboy::BoxedConsumerQueue<int64_t> rq;
    rq.SetImpl(qq);
    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
        [&](size_t batchSize) {
            int64_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                wq.Put(i);
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                rq.Take();
            }
        });
}
BENCHMARK(Int64Virtual);


static void StringVirtual(benchmark::State& state) {
    auto qq = std::make_shared<postboy::SPSCQueue<std::string>>();
    postboy::BoxedProducerQueue<std::string> wq;
    wq.SetImpl(qq);
    postboy::BoxedConsumerQueue<std::string> rq;
    rq.SetImpl(qq);

    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                wq.Put({});
            }
        },
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                rq.Take();
            }
        });
}
BENCHMARK(StringVirtual);

BENCHMARK_MAIN();
