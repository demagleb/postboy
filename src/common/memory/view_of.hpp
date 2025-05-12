#pragma once

#include <common/memory/view.hpp>
#include <common/util/types.hpp>

#include <string>
#include <vector>
#include <cstring>

namespace common::memory {

//////////////////////////////////////////////////////////////////////

template <size_t N>
FORCE_INLINE ConstMemView ViewOf(char (&arr)[N]) {
    return {arr, N};
}

FORCE_INLINE ConstMemView ViewOf(const char* str) {
    return {str, strlen(str)};
}

FORCE_INLINE ConstMemView ViewOf(const std::string& str) {
    return {str.data(), str.length()};
}

FORCE_INLINE ConstMemView ViewOf(const std::vector<char>& bytes) {
    return {bytes.data(), bytes.size()};
}

//////////////////////////////////////////////////////////////////////

template <size_t N>
FORCE_INLINE MutableMemView MutViewOf(char (&arr)[N]) {
    return {arr, N};
}

FORCE_INLINE MutableMemView MutViewOf(std::string& str) {
    return {str.data(), str.length()};
}

FORCE_INLINE MutableMemView MutViewOf(std::vector<char>& bytes) {
    return {bytes.data(), bytes.size()};
}

} // namespace common::memory
