#include <gtest/gtest.h>

#include <common/system/cpu_helper.hpp>

#include <iostream>

TEST(CpuHelper, SimpleCpuHelper) {
    std::cout << common::system::CpuIdHelper::GetProcessorDescription() << std::endl;
    std::cout << common::system::CpuIdHelper::GetCacheLineSize() << std::endl;
    std::cout << common::system::CpuIdHelper::IsInvariantTsc() << std::endl;
}
