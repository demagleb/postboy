#include "benchmark_queue.h"

#include <common/concurrency/moodycamel_readerwriter_queue.hpp>

#include <cstdint>
#include <thread>

constexpr size_t kQueueSize = 512;


static void Int64(benchmark::State& state) {
    moodycamel::ReaderWriterQueue<int64_t> q(kQueueSize);
    BenchmarkQueue(
        state,
        1,
        1,
        2048,
        [&](size_t batchSize) {
            int64_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                while (!q.try_enqueue(++i)) {
                    _mm_pause();
                }
            }
        },
        [&](size_t batchSize) {
            int64_t i = 0;
            for (size_t j = 0; j < batchSize; ++j) {
                while (!q.try_dequeue(i)) {
                    _mm_pause();
                }
            }
        });
}
BENCHMARK(Int64)->Repetitions(100000)->Iterations(2048)->ReportAggregatesOnly(true);


static void String(benchmark::State& state) {
    moodycamel::ReaderWriterQueue<std::string> q(kQueueSize);
    BenchmarkQueue(
        state,
        1,
        1,
        2048,
        [&](size_t batchSize) {
            for (size_t j = 0; j < batchSize; ++j) {
                while (!q.try_enqueue({})) {
                    _mm_pause();
                }
            }
        },
        [&](size_t batchSize) {
            std::string i;
            for (size_t j = 0; j < batchSize; ++j) {
                while (!q.try_dequeue(i)) {
                    _mm_pause();
                }
            }
        });
}
BENCHMARK(String)->Repetitions(100000)->Iterations(2048)->ReportAggregatesOnly(true);;

BENCHMARK_MAIN();
