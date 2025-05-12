#pragma once

namespace common::util {

struct NonMovable {
    NonMovable() = default;

    NonMovable(NonMovable&&) = delete;
    NonMovable(const NonMovable&) = delete;

    NonMovable& operator=(NonMovable&&) = delete;
    NonMovable& operator=(const NonMovable&) = delete;
};

struct MoveOnly {
    MoveOnly() = default;

    MoveOnly(MoveOnly&&) = default;
    MoveOnly(const MoveOnly&) = delete;

    MoveOnly& operator=(MoveOnly&&) = default;
    MoveOnly& operator=(const MoveOnly&) = delete;
};

} // namespace common::util
