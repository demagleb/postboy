#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace common::util {

template <class Tag, class T>
class Tagged {
public:
    using TagType = Tag;
    using ValueType = T;

    Tagged()
        : value_() {
    }

    constexpr explicit Tagged(T value)
        : value_(std::move(value)) {
    }

    template <class S>
    constexpr explicit Tagged(const Tagged<Tag, S>& value)
        : value_(value.Value()) {
    }

    constexpr const T& Value() const& {
        return value_;
    }
    constexpr const T&& Value() const&& {
        return std::move(value_);
    }
    constexpr T& Value() & {
        return value_;
    }
    constexpr T&& Value() && {
        return std::move(value_);
    }

    constexpr explicit operator const T&() const& {
        return value_;
    }
    constexpr explicit operator T&&() && {
        return std::move(value_);
    }

    bool operator==(const Tagged&) const = default;
    auto operator<=>(const Tagged&) const = default;

    template <typename OtherValue>
    auto operator==(const Tagged<Tag, OtherValue>& otherTagged) const
        -> decltype(this->Value() == otherTagged.Value()) {
        return Value() == otherTagged.Value();
    }

    template <typename OtherValue>
    auto operator<=>(const Tagged<Tag, OtherValue>& otherTagged) const
        -> decltype(this->Value() <=> otherTagged.Value()) {
        return Value() <=> otherTagged.Value();
    }

    bool operator<(const Tagged& other) const {
        return value_ < other.value_;
    }

    bool operator>(const Tagged& other) const {
        return value_ > other.value_;
    }

    bool operator<=(const Tagged& other) const {
        return value_ <= other.value_;
    }

    bool operator>=(const Tagged& other) const {
        return value_ >= other.value_;
    }

    Tagged& operator++() {
        ++value_;
        return *this;
    }

    Tagged operator++(int) {
        Tagged result = *this;
        value_++;
        return result;
    }

    Tagged& operator--() {
        --value_;
        return *this;
    }

    Tagged operator--(int) {
        Tagged result = *this;
        value_--;
        return result;
    }

    Tagged& operator+=(const Tagged& other) {
        value_ += other.value_;
        return *this;
    }

    Tagged& operator-=(const Tagged& other) {
        value_ -= other.value_;
        return *this;
    }

    constexpr Tagged operator+(T x) const {
        return Tagged(value_ + x);
    }

    constexpr Tagged operator-(T x) const {
        return Tagged(value_ - x);
    }

    constexpr Tagged operator+(const Tagged& other) const {
        return Tagged(value_ + other.value_);
    }

    constexpr Tagged operator-(const Tagged& other) const {
        return Tagged(value_ - other.value_);
    }

    constexpr Tagged operator-() const {
        return Tagged(-value_);
    }

    template <class S>
    constexpr Tagged operator*(S mult) const {
        static_assert(std::is_arithmetic_v<S>);
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<S>);
        return Tagged(value_ * mult);
    }

    template <class S>
    constexpr Tagged operator/(S div) const {
        static_assert(std::is_arithmetic_v<S>);
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<S>);
        return Tagged(value_ / div);
    }

    template <class S>
    constexpr Tagged operator%(S div) const {
        static_assert(std::is_integral_v<T> && std::is_integral_v<S>);
        return Tagged(value_ % div);
    }

    constexpr T operator/(const Tagged& t) const {
        return (value_ / t.value_);
    }

    constexpr Tagged operator%(const Tagged& t) const {
        return Tagged(value_ % t.value_);
    }

    auto introspect() const { // NOLINT
        return std::tie(value_);
    }

private:
    T value_;
};

} // namespace common::util
