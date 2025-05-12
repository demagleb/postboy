#pragma once

#include <common/util/types.hpp>

#include <array>
#include <ctime>
#include <string>

#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>

namespace common::util {

std::string GetCurrentDate();

FORCE_INLINE uint64_t NowNs() {
    timespec now{.tv_sec = 0, .tv_nsec = 0};

    clock_gettime(CLOCK_REALTIME, &now);
    uint64_t res = now.tv_sec;
    res *= 1000000000;
    res += now.tv_nsec;
    return res;
}

FORCE_INLINE uint64_t NowMs() {
    timespec now{.tv_sec = 0, .tv_nsec = 0};

    clock_gettime(CLOCK_REALTIME, &now);
    uint64_t res = now.tv_sec;
    res *= 1000000;
    res += now.tv_nsec;
    return res;
}

} // namespace common::util
