#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <utility>

namespace common::util {

namespace detail {

template <bool moveLeft, bool moveRight, size_t leftSize, size_t rightSize, class T, class LArray, class RArray,
          size_t... leftIxs, size_t... rightIxs>
constexpr auto Concat(LArray&& left, RArray&& right, std::integer_sequence<size_t, leftIxs...>,
                      std::integer_sequence<size_t, rightIxs...>) -> std::array<T, leftSize + rightSize> {
    if constexpr (moveLeft && moveRight) {
        return {{std::move(left[leftIxs])..., std::move(right[rightIxs])...}};
    } else if constexpr (moveLeft && !moveRight) {
        return {{std::move(left[leftIxs])..., right[rightIxs]...}};
    } else if constexpr (!moveLeft && moveRight) {
        return {{left[leftIxs]..., std::move(right[rightIxs])...}};
    } else if constexpr (!moveLeft && !moveRight) {
        return {{left[leftIxs]..., right[rightIxs]...}};
    }

    std::terminate();
}

template <class T, size_t... ixs>
constexpr std::array<T, sizeof...(ixs)> MakeArray(const T& initValue, std::integer_sequence<size_t, ixs...>) {
    auto getValue = [&initValue](auto) -> const T& { return initValue; };

    return {{getValue(ixs)...}};
}

} // namespace detail

template <size_t rightSize, class T, size_t leftSize>
constexpr std::array<T, leftSize + rightSize> Concat(const std::array<T, leftSize>& left,
                                                     const std::array<T, rightSize>& right) {
    return detail::Concat<false, false, leftSize, rightSize, T>(
        left, right, std::make_integer_sequence<size_t, leftSize>(), std::make_integer_sequence<size_t, rightSize>());
}

template <size_t rightSize, class T, size_t leftSize>
constexpr std::array<T, leftSize + rightSize> Concat(std::array<T, leftSize>&& left, std::array<T, rightSize>&& right) {
    return detail::Concat<true, true, leftSize, rightSize, T>(
        left, right, std::make_integer_sequence<size_t, leftSize>(), std::make_integer_sequence<size_t, rightSize>());
}

template <size_t rightSize, class T, size_t leftSize>
constexpr std::array<T, leftSize + rightSize> Concat(const std::array<T, leftSize>& left,
                                                     std::array<T, rightSize>&& right) {
    return detail::Concat<false, true, leftSize, rightSize, T>(
        left, right, std::make_integer_sequence<size_t, leftSize>(), std::make_integer_sequence<size_t, rightSize>());
}

template <size_t rightSize, class T, size_t leftSize>
constexpr std::array<T, leftSize + rightSize> Concat(std::array<T, leftSize>&& left,
                                                     const std::array<T, rightSize>& right) {
    return detail::Concat<true, false, leftSize, rightSize, T>(
        left, right, std::make_integer_sequence<size_t, leftSize>(), std::make_integer_sequence<size_t, rightSize>());
}

template <size_t size, class T>
constexpr std::array<T, size> MakeArray(const T& initValue) {
    return detail::MakeArray(initValue, std::make_integer_sequence<size_t, size>());
}

} // namespace common::util
