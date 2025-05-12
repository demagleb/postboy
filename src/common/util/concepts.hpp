#pragma once

namespace common::util {

constexpr bool PowerOf2(int number) {
    return (number & (number - 1)) == 0;
}

} // namespace common::util
