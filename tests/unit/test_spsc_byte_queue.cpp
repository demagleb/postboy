#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <common/system/high_resolution_clock.hpp>
#include <common/util/quantiles.hpp>
#include <common/concurrency/spsc_byte_queue.hpp>

TEST(SpscByteQueue, SimpleOperations) {
    struct Packet {
        int a;
        int64_t b;
    };

    common::concurrency::SpscByteQueue q(sizeof(Packet) * 1000 / sizeof(char), sizeof(Packet));

    ASSERT_TRUE(q.Empty());
    ASSERT_TRUE(q.Available());

    std::thread t1([&q]() {
        for (int i = 0; i < 100000; ++i) {
            Packet packet;
            packet.a = 1;
            packet.b = 2;
            q.Store(reinterpret_cast<char*>(&packet), sizeof(Packet));
        }
    });

    std::thread t2([&q]() {
        for (int i = 0; i < 100000; ++i) {
            Packet packet;
            q.Load(reinterpret_cast<char*>(&packet), sizeof(Packet));
            ASSERT_EQ(packet.a, 1);
            ASSERT_EQ(packet.b, 2);
        }
    });

    t1.join();
    t2.join();
}

TEST(SpscByteQueue, OpeationTestTime) {
    common::system::TimeRdtsc::Init();
    common::system::TimeRdtsc cpuTimer;

    static constexpr size_t kQueueSize = 1e6;
    common::concurrency::SpscByteQueue q(sizeof(int) * kQueueSize / sizeof(char), sizeof(int));

    std::vector<uint64_t> resultTime;
    int k = 1;
    for (size_t i = 0; i < kQueueSize; ++i) {
        cpuTimer.Start();
        q.Store(reinterpret_cast<char*>(&k), sizeof(k));
        cpuTimer.Stop();

        resultTime.push_back(cpuTimer.GetElapsedTicks());
    }

    std::sort(resultTime.begin(), resultTime.end());
    std::vector<int> quantilesValues{0, 5, 10, 25, 50, 60, 70, 80, 90, 95, 99, 100};
    auto quantiles = common::util::Calculate<uint64_t>(resultTime.begin(), resultTime.end(), quantilesValues);
    std::stringstream ss;
    ss << "PUSH: ";
    common::util::PrintWithIdx(ss, quantiles);

    resultTime.clear();
    for (size_t i = 0; i < kQueueSize; ++i) {
        cpuTimer.Start();
        q.Load(reinterpret_cast<char*>(&k), sizeof(k));
        cpuTimer.Stop();

        resultTime.push_back(cpuTimer.GetElapsedTicks());
    }

    std::sort(resultTime.begin(), resultTime.end());
    quantiles = common::util::Calculate<uint64_t>(resultTime.begin(), resultTime.end(), quantilesValues);
    ss << "POP: ";
    common::util::PrintWithIdx(ss, quantiles);

    std::cerr << ss.str();
}

TEST(SpscByteQueue, TryLoadTest) {
    static constexpr size_t kQueueSize = 1;
    static constexpr int kElement = 2;

    common::concurrency::SpscByteQueue q(sizeof(int) * kQueueSize / sizeof(char), sizeof(int));
    q.Store(reinterpret_cast<const char*>(&kElement), sizeof(kElement));

    ASSERT_FALSE(q.TryStore(reinterpret_cast<const char*>(&kElement), sizeof(kElement)));

    {
        int element = 0xdeadbeef;
        ASSERT_TRUE(q.TryLoad(reinterpret_cast<char*>(&element), sizeof(int)));
        ASSERT_EQ(element, kElement);
    }

    {
        int element = 0xdeadbeef;
        ASSERT_FALSE(q.TryLoad(reinterpret_cast<char*>(&element), sizeof(int)));
        ASSERT_EQ(element, 0xdeadbeef);
    }

    ASSERT_TRUE(q.TryStore(reinterpret_cast<const char*>(&kElement), sizeof(kElement)));
}
