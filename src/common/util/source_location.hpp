#pragma once

#include <iostream>
#include <string>
#include <string_view>

namespace common::util {

// Represents location in source code: file name, function name, line number

class SourceLocation {
public:
    SourceLocation() = default;

    SourceLocation(std::string_view file, std::string_view function, int line)
        : file_(file)
        , function_(function)
        , line_(line) {
    }

    static SourceLocation Current(const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(),
                                  const int line = __builtin_LINE()) {
        return {file, function, line};
    }

    std::string_view File() const {
        return file_;
    }

    std::string_view Function() const {
        return function_;
    }

    int Line() const {
        return line_;
    }

private:
    std::string_view file_;
    std::string_view function_;
    int line_;
};

std::ostream& operator<<(std::ostream& out, const SourceLocation& where);

inline SourceLocation Here(SourceLocation loc = SourceLocation::Current()) {
    return loc;
}

} // namespace common::util

#define CODE_HERE ::common::util::SourceLocation(__FILE__, __PRETTY_FUNCTION__, __LINE__)
