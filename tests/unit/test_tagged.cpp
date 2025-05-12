#include <gtest/gtest.h>

#include <common/util/tagged.hpp>

using namespace std::chrono_literals;

TEST(Tagged, SimpleOperations) {
    struct MyIntTag;
    common::util::Tagged<MyIntTag, int> firstValue(5);
    common::util::Tagged<MyIntTag, int> secondValue(6);

    ASSERT_TRUE(firstValue.Value() == 5);
    ASSERT_TRUE(secondValue.Value() == 6);

    auto res = firstValue + secondValue;

    ASSERT_TRUE(res.Value() == 11);

    res = firstValue - secondValue;

    ASSERT_TRUE(res.Value() == -1);
    ASSERT_TRUE(--res.Value() == -2);
    ASSERT_TRUE(++res.Value() == -1);
    ASSERT_TRUE(res--.Value() == -1);
    ASSERT_TRUE(res++.Value() == -2);
    ASSERT_TRUE(res.Value() == -1);

    res += firstValue;
    res -= secondValue;

    ASSERT_TRUE(res.Value() == -2);
}

TEST(Tagged, DifferentTypesUnderEqualTag) {
    struct MyIntTag;
    common::util::Tagged<MyIntTag, int> firstValue(5);
    common::util::Tagged<MyIntTag, int64_t> secondValue(6);

    ASSERT_TRUE(firstValue != secondValue);
    ASSERT_TRUE(firstValue < secondValue);
    ASSERT_TRUE(firstValue <= secondValue);
    ASSERT_FALSE(firstValue > secondValue);
    ASSERT_FALSE(firstValue >= secondValue);
}
