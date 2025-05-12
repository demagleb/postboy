#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <common/system/binary_dir.hpp>

using ::testing::HasSubstr;
using ::testing::Not;

TEST(System, CurrentBinaryDir) {
    static const auto kCurrentBinaryDir = common::system::GetCurrentBinaryDir();

    EXPECT_THAT(kCurrentBinaryDir.c_str(), HasSubstr("tests/unit"));
    EXPECT_THAT(kCurrentBinaryDir.c_str(), Not(HasSubstr("tests/unit/test_system")));
}
