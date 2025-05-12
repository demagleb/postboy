#include "benchmark_queue.h"

#include <boost/lockfree/queue.hpp>

constexpr size_t kQueueSize = 512;

static void Int64P1C1(benchmark::State& state) {
    boost::lockfree::queue<int64_t> q(kQueueSize);
    BenchmarkQueue(
        state,
        1,
        1,
        10240,
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.push(j);
            }
        },
        [&](size_t batchSize) {
            int64_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                while (!q.pop(i)) {
                    std::this_thread::yield();
                }
            }
        });
}
BENCHMARK(Int64P1C1)->Repetitions(10000)->Iterations(10240)->ReportAggregatesOnly(true);


static void Int64P3C1(benchmark::State& state) {
    boost::lockfree::queue<int64_t> q(kQueueSize);
    BenchmarkQueue(
        state,
        3,
        1,
        10240,
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.push(j);
            }
        },
        [&](size_t batchSize) {
            int64_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                while (!q.pop(i)) {
                    std::this_thread::yield();
                }
            }
        });
}
BENCHMARK(Int64P3C1)->Repetitions(10000)->Iterations(30720)->ReportAggregatesOnly(true);


// static void StringP3C1(benchmark::State& state) {
//     boost::lockfree::queue<std::string> q(10240);
//     BenchmarkQueue(
//         state,
//         3,
//         1,
//         10240,
//         [&](size_t batchSize) {
//             for (size_t j = 0; j < batchSize; ++j) {
//                 q.push({});
//             }
//         },
//         [&](size_t batchSize) {
//             std::string i;
//             for (size_t j = 0; j < batchSize; ++j) {
//                 while (!q.pop(i)) {
//                     std::this_thread::yield();
//                 }
//             }
//         });
// }
// BENCHMARK(StringP3C1)->Repetitions(10000)->Iterations(30720)->ReportAggregatesOnly(true);

static void Int64P1C3(benchmark::State& state) {
    boost::lockfree::queue<int64_t> q(kQueueSize);
    BenchmarkQueue(
        state,
        1,
        3,
        10240,
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.push(j);
            }
        },
        [&](size_t batchSize) {
            int64_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                while (!q.pop(i)) {
                    _mm_pause();
                }
            }
        });
}
BENCHMARK(Int64P1C3)->Repetitions(10000)->Iterations(10240)->ReportAggregatesOnly(true);


// static void StringP1C3(benchmark::State& state) {
//     boost::lockfree::queue<std::string> q(10240);
//     BenchmarkQueue(
//         state,
//         1,
//         3,
//         10240,
//         [&](size_t batchSize) {
//             for (size_t j = 0; j < batchSize; ++j) {
//                 q.push({});
//             }
//         },
//         [&](size_t batchSize) {
//             std::string i;
//             for (size_t j = 0; j < batchSize; ++j) {
//                 while (!q.pop(i)) {
//                     std::this_thread::yield();
//                 }
//             }
//         });
// }
// BENCHMARK(StringP1C3)->Repetitions(10000)->Iterations(10240)->ReportAggregatesOnly(true);


static void Int64P3C3(benchmark::State& state) {
    boost::lockfree::queue<int64_t> q(kQueueSize);
    BenchmarkQueue(
        state,
        3,
        3,
        10240,
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                q.push(j);
            }
        },
        [&](size_t batchSize) {
            int64_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                while (!q.pop(i)) {
                    _mm_pause();
                }
            }
        });
}
BENCHMARK(Int64P3C3)->Repetitions(10000)->Iterations(30720)->ReportAggregatesOnly(true);


// static void StringP3C3(benchmark::State& state) {
//     boost::lockfree::queue<std::string> q(10240);
//     BenchmarkQueue(
//         state,
//         3,
//         3,
//         10240,
//         [&](size_t batchSize) {
//             for (size_t j = 0; j < batchSize; ++j) {
//                 q.push({});
//             }
//         },
//         [&](size_t batchSize) {
//             std::string i;
//             for (size_t j = 0; j < batchSize; ++j) {
//                 while (!q.pop(i)) {
//                     std::this_thread::yield();
//                 }
//             }
//         });
// }
// BENCHMARK(StringP3C3)->Repetitions(10000)->Iterations(30720)->ReportAggregatesOnly(true);

BENCHMARK_MAIN();
