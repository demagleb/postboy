#pragma once

#include <common/util/source_location.hpp>

#include <sstream>
#include <string>

namespace common::util {

namespace detail {

[[noreturn]] void Panic(common::util::SourceLocation where, const std::string& error);
[[noreturn]] void Throw(common::util::SourceLocation where, const std::string& error);
void Info(common::util::SourceLocation where, const std::string& error);
void Warn(common::util::SourceLocation where, const std::string& error);

} // namespace detail

} // namespace common::util

// Usage: PANIC("Internal error: " << e.what());

#define PANIC(error)                                                                    \
    do {                                                                                \
        ::common::util::detail::Panic(CODE_HERE, (std::stringstream() << error).str()); \
    } while (false)

#define INFO(text)                                                                    \
    do {                                                                              \
        ::common::util::detail::Info(CODE_HERE, (std::stringstream() << text).str()); \
    } while (false)

#define WARN(warning)                                                                    \
    do {                                                                                 \
        ::common::util::detail::Warn(CODE_HERE, (std::stringstream() << warning).str()); \
    } while (false)
