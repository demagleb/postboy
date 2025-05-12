#include <gtest/gtest.h>

#include <common/util/defer.hpp>

TEST(Defer, SimpleTest) {
    testing::internal::CaptureStdout();
    {
        defer {
            std::cout << 39 << std::endl;
            defer {
                std::cout << 40 << std::endl;
                defer {
                    std::cout << 41 << std::endl;
                    defer {
                        std::cout << 42 << std::endl;
                    };
                };
            };
        };
        defer {
            std::cout << 43 << std::endl;
        };
    }
    const auto output = testing::internal::GetCapturedStdout();
    EXPECT_STREQ(output.c_str(), "43\n39\n40\n41\n42\n");
}
