#include <gtest/gtest.h>

#include <common/util/constexpr_array.hpp>

TEST(ConstExprArray, SimpleMakeArrayTest) {
    constexpr auto kTemp1 = common::util::MakeArray<5, int>(10);
    static_assert(kTemp1[0] == 10);
}

TEST(ConstExprArray, SimpleConcatTest) {
    constexpr auto kTemp1 = common::util::MakeArray<5, int>(10);
    constexpr auto kTemp2 = common::util::MakeArray<5, int>(11);
    constexpr auto kTemp3 = common::util::Concat(kTemp1, kTemp2);
    static_assert(kTemp3[0] == 10);
    static_assert(kTemp3[5] == 11);
}
