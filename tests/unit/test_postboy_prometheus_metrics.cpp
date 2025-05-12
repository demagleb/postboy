#include <gtest/gtest.h>

#include <common/concurrency/spinlock.hpp>
#include <common/system/high_resolution_clock.hpp>

#include <postboy/prometheus_metrics/counter.hpp>
#include <postboy/prometheus_metrics/gauge.hpp>
#include <postboy/prometheus_metrics/metric_types.hpp>
#include <postboy/prometheus_metrics/registry.hpp>

#include <functional>
#include <memory>
#include <thread>

using namespace std::chrono_literals;

TEST(PostboyPrometheusMetrics, CreateMetrics) {
    auto registry = std::make_unique<postboy::prometheus::Registry>();

    auto& counterBuilder =
        postboy::prometheus::BuildCounter().Name("NewCounter").Help("HelpCounter").Register(*registry);

    EXPECT_EQ(counterBuilder.GetName(), "NewCounter");

    auto& firstCounter = counterBuilder.Add({{"key1", "value1"}, {"key2", "value2"}});

    for (auto i = 0; i < 15; ++i) {
        firstCounter.Increment();
    }
    firstCounter.Reset();
}

TEST(PostboyPrometheusMetrics, CreateFamilyWithSameName) {
    auto registry = std::make_unique<postboy::prometheus::Registry>();

    auto& counterBuilder =
        postboy::prometheus::BuildCounter().Name("NewCounter").Help("HelpCounter").Register(*registry);

    EXPECT_EQ(counterBuilder.GetName(), "NewCounter");

    ASSERT_DEATH(postboy::prometheus::BuildCounter().Name("NewCounter").Register(*registry), "");

    auto& firstCounter = counterBuilder.Add({{"key1", "value1"}, {"key2", "value2"}});
    for (auto i = 0; i < 15; ++i) {
        firstCounter.Increment();
    }
    firstCounter.Reset();
}

TEST(PostboyPrometheusMetrics, RemoveFamilyFromRegistry) {
    auto registry = std::make_unique<postboy::prometheus::Registry>();

    for (int i = 0; i < 15; ++i) {
        auto& firstCounterBuilder = postboy::prometheus::BuildCounter()
                                        .Name("NewCounter" + std::to_string(i))
                                        .Help("HelpCounter")
                                        .Register(*registry);

        EXPECT_EQ(firstCounterBuilder.GetName(), "NewCounter" + std::to_string(i));

        auto& secondCounterBuilder = postboy::prometheus::BuildCounter()
                                         .Name("NewCounter" + std::to_string(i + 1))
                                         .Help("HelpCounter")
                                         .Register(*registry);

        EXPECT_EQ(secondCounterBuilder.GetName(), "NewCounter" + std::to_string(i + 1));

        registry->Remove(firstCounterBuilder);
        registry->Remove(secondCounterBuilder);
    }
}

TEST(PostboyPrometheusMetrics, AddHasRemoveFamilyOperations) {
    auto registry = std::make_unique<postboy::prometheus::Registry>();

    auto& counterBuilder =
        postboy::prometheus::BuildCounter().Name("NewCounter").Help("HelpCounter").Register(*registry);

    std::vector<std::reference_wrapper<postboy::prometheus::Counter>> counters;
    for (int i = 0; i < 15; ++i) {
        auto& counter = counterBuilder.Add({{"key" + std::to_string(i), "value" + std::to_string(i)}});
        counters.push_back(counter);
    }

    for (int i = 0; i < 15; ++i) {
        EXPECT_TRUE(counterBuilder.Has({{"key" + std::to_string(i), "value" + std::to_string(i)}}));

        counterBuilder.Remove(&counters[i].get());
    }

    for (int i = 0; i < 100; ++i) {
        EXPECT_FALSE(counterBuilder.Has({{"key" + std::to_string(i), "value" + std::to_string(i)}}));
    }
}

TEST(PostboyPrometheusMetrics, CounterConcurrentOperations) {
    auto registry = std::make_unique<postboy::prometheus::Registry>();

    auto& counterBuilder =
        postboy::prometheus::BuildCounter().Name("NewCounter").Help("HelpCounter").Register(*registry);

    auto& counter = counterBuilder.Add({{"key1", "value1"}, {"key2", "value2"}});
    EXPECT_EQ(counter.MetricType(), postboy::prometheus::MetricType::Counter);

    auto t1 = std::thread([&counter]() {
        for (int i = 0; i < 1e6; ++i) {
            counter.Increment();
        }
    });

    double prevValue = 0.0;
    while (true) {
        auto newValue = counter.Collect().counter.value;
        EXPECT_LE(prevValue, newValue);
        prevValue = newValue;

        if (prevValue == 1e6) {
            break;
        }
    }

    t1.join();

    counter.Reset();
    EXPECT_EQ(counter.Collect().counter.value, 0.0);
}

TEST(PostboyPrometheusMetrics, GaugeConcurrentOperations) {
    auto registry = std::make_unique<postboy::prometheus::Registry>();

    auto& gaugeBuilder = postboy::prometheus::BuildGauge().Name("NewGauge").Help("HelpGauge").Register(*registry);

    auto& gauge = gaugeBuilder.Add({{"key1", "value1"}, {"key2", "value2"}});
    EXPECT_EQ(gauge.MetricType(), postboy::prometheus::MetricType::Gauge);

    auto t1 = std::thread([&gauge]() {
        for (int i = 0; i < 1e6; ++i) {
            gauge.Increment(2.0);
        }
    });

    double prevValue = 0.0;
    while (true) {
        auto newValue = gauge.Collect().gauge.value;
        EXPECT_LE(prevValue, newValue);
        prevValue = newValue;

        if (prevValue == 2.0 * 1e6) {
            break;
        }
    }

    t1.join();

    auto t2 = std::thread([&gauge]() {
        for (int i = 0; i < 1e6; ++i) {
            gauge.Decrement(2.0);
        }
    });

    while (true) {
        auto newValue = gauge.Collect().gauge.value;
        EXPECT_LE(newValue, prevValue);
        prevValue = newValue;

        if (prevValue == 0) {
            break;
        }
    }

    t2.join();

    gauge.Set(1.1);
    EXPECT_EQ(gauge.Collect().gauge.value, 1.1);
}
