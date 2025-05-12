#include "benchmark_queue.h"

#include <postboy/base/queues/queue.hpp>

static void Int32P1C1(benchmark::State& state) {
    postboy::MPMCQueue<int32_t> q;
    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
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
BENCHMARK(Int32P1C1);

static void Int64P1C1(benchmark::State& state) {
    postboy::MPMCQueue<int64_t> q;
    BenchmarkQueue(
        state,
        1,
        1,
        1'000'000,
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
BENCHMARK(Int64P1C1);


static void StringP1C1(benchmark::State& state) {
    postboy::MPMCQueue<std::string> q;
    BenchmarkQueue(
        state,
        3,
        1,
        1'000'000,
        [&](size_t batchSize) {
            int64_t i = 0;
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
BENCHMARK(StringP1C1);

static void Int32P3C1(benchmark::State& state) {
    postboy::MPMCQueue<int32_t> q;
    BenchmarkQueue(
        state,
        3,
        1,
        1'000'000,
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
BENCHMARK(Int32P3C1);

static void Int64P3C1(benchmark::State& state) {
    postboy::MPMCQueue<int64_t> q;
    BenchmarkQueue(
        state,
        3,
        1,
        1'000'000,
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
BENCHMARK(Int64P3C1);


static void StringP3C1(benchmark::State& state) {
    postboy::MPMCQueue<std::string> q;
    BenchmarkQueue(
        state,
        3,
        1,
        1'000'000,
        [&](size_t batchSize) {
            int64_t i = 0;
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
BENCHMARK(StringP3C1);

static void Int32P1C3(benchmark::State& state) {
    postboy::MPMCQueue<int32_t> q;
    BenchmarkQueue(
        state,
        1,
        3,
        1'000'000,
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
BENCHMARK(Int32P3C1);

static void Int64P1C3(benchmark::State& state) {
    postboy::MPMCQueue<int64_t> q;
    BenchmarkQueue(
        state,
        1,
        3,
        1'000'000,
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
BENCHMARK(Int64P3C1);


static void StringP1C3(benchmark::State& state) {
    postboy::MPMCQueue<std::string> q;
    BenchmarkQueue(
        state,
        1,
        3,
        1'000'000,
        [&](size_t batchSize) {
            int64_t i = 0;
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
BENCHMARK(StringP3C1);


static void Int32P3C3(benchmark::State& state) {
    postboy::MPMCQueue<int32_t> q;
    BenchmarkQueue(
        state,
        3,
        3,
        1'000'000,
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
BENCHMARK(Int32P3C3);

static void Int64P3C3(benchmark::State& state) {
    postboy::MPMCQueue<int64_t> q;
    BenchmarkQueue(
        state,
        3,
        3,
        1'000'000,
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
BENCHMARK(Int64P3C3);


static void StringP3C3(benchmark::State& state) {
    postboy::MPMCQueue<std::string> q;
    BenchmarkQueue(
        state,
        3,
        3,
        1'000'000,
        [&](size_t batchSize) {
            int64_t i = 0;
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
BENCHMARK(StringP3C3);

BENCHMARK_MAIN();
