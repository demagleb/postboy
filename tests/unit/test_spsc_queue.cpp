#include <gtest/gtest.h>

#include <chrono>
#include <ratio>
#include <thread>

#include <common/system/high_resolution_clock.hpp>
#include <common/util/quantiles.hpp>
#include <common/concurrency/spsc_queue.hpp>

TEST(SpscQueue, SimpleOperations) {
    common::concurrency::SpscQueue<int, 100> q;

    std::thread t1([&q]() {
        for (int i = 0; i < 100000; ++i) {
            q.Push(i);
        }
        q.Close();
    });

    std::thread t2([&q]() {
        for (int i = 0; i < 100000; ++i) {
            auto res = q.Pop();
            ASSERT_TRUE(res);
            ASSERT_EQ(*res, i);
        }
        ASSERT_FALSE(q.Pop());
        ASSERT_TRUE(q.IsClosed());
    });

    t1.join();
    t2.join();
}

TEST(SpscQueue, SlowPop) {
    common::concurrency::SpscQueue<int, 100> q;

    std::thread t1([&q]() {
        for (int i = 0; i < 1000; ++i) {
            q.Push(i);
        }
        q.Close();
    });

    std::thread t2([&q]() {
        for (int i = 0; i < 1000; ++i) {
            auto res = q.Pop();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            ASSERT_TRUE(res);
            ASSERT_EQ(*res, i);
        }
        ASSERT_FALSE(q.Pop());
        ASSERT_TRUE(q.IsClosed());
    });

    t1.join();
    t2.join();
}

TEST(SpscQueue, SlowPush) {
    common::concurrency::SpscQueue<int, 100> q;

    std::thread t1([&q]() {
        for (int i = 0; i < 1000; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            q.Push(i);
        }
        q.Close();
    });

    std::thread t2([&q]() {
        for (int i = 0; i < 1000; ++i) {
            auto res = q.Pop();
            ASSERT_TRUE(res);
            ASSERT_EQ(*res, i);
        }
        ASSERT_FALSE(q.Pop());
        ASSERT_TRUE(q.IsClosed());
    });

    t1.join();
    t2.join();
}
