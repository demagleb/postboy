#pragma once

#include <atomic>
#include <benchmark/benchmark.h>
#include <common/util/types.hpp>
#include <common/concurrency/spinlock.hpp>

#include <mutex>
#include <thread>
#include <iostream>

template <typename Push, typename Pop>
FORCE_INLINE void BenchmarkQueue(
    benchmark::State& state,
    const size_t producers,
    const size_t consumers,
    const size_t batchSize,
    Push&& push,
    Pop&& pop)
{
    std::atomic<size_t> producersStopped{0};
    std::atomic<size_t> batchesProduced{0};
    std::atomic<size_t> batchesConsumed{0};

    common::concurrency::Spinlock mutex1;
    auto keepRunningProducer = [&] {
        std::lock_guard lock{mutex1};
        if (state.KeepRunningBatch(batchSize)) {
            batchesProduced.fetch_add(1, std::memory_order_relaxed);
            return true;
        }
        producersStopped.fetch_add(1, std::memory_order_relaxed);
        return false;
    };

    common::concurrency::Spinlock mutex2;
    auto keepRunningConsumer = [&] {
        std::lock_guard lock{mutex2};
        size_t ps = producersStopped.load(std::memory_order_relaxed);
        size_t bc = batchesConsumed.load(std::memory_order_relaxed);
        size_t bp = batchesProduced.load(std::memory_order_relaxed);
        while (ps != producers && bc == bp) {
            _mm_pause();
            ps = producersStopped.load(std::memory_order_relaxed);
            bc = batchesConsumed.load(std::memory_order_relaxed);
            bp = batchesProduced.load(std::memory_order_relaxed);
        }
        if (bc < bp) {
            batchesConsumed.fetch_add(1, std::memory_order_relaxed);
            return true;
        }
        return false;
    };

    std::vector<std::jthread> threads;
    for (size_t i = 0; i < producers; ++i) {
        threads.emplace_back([&] {
            while (keepRunningProducer()) {
                push(batchSize);
            }
        });
    }

    for (size_t i = 0; i < consumers; ++i) {
        threads.emplace_back([&] {
            while (keepRunningConsumer()) {
                pop(batchSize);
            }
        });
    }
}
