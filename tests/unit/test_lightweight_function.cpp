#include <gtest/gtest.h>

#include <common/util/lightweight_function.hpp>

using namespace std::chrono_literals;

TEST(LightweightFunc, Simple) {
    int x = 1;
    auto func = common::util::LightweightFunc<int(int)>([&x](int a) {
        x++;
        return a + 1;
    });

    EXPECT_TRUE(func);
    EXPECT_EQ(func(10), 11);
    EXPECT_EQ(x, 2);

    auto func1 = common::util::LightweightFunc<int(int)>();
    EXPECT_FALSE(func1);
}

TEST(LightweightFunc, Copy) {
    int x = 1;
    auto func = common::util::LightweightFunc<int(int)>([&x](int a) {
        x++;
        return a + 1;
    });

    auto func2 = func;

    EXPECT_TRUE(func);
    EXPECT_EQ(func(10), 11);
    EXPECT_EQ(x, 2);

    EXPECT_TRUE(func2);
    EXPECT_EQ(func2(10), 11);
    EXPECT_EQ(x, 3);
}