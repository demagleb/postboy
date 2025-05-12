#pragma once

#include <common/util/panic.hpp>
#include <common/util/types.hpp>

/* Use-cases:
 * 1) Contract violations
 * 2) Unrecoverable errors
 */

////////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG

    #define ASSERT(cond, error)                                         \
        do {                                                            \
            if (!(cond)) { /*NOLINT*/                                   \
                PANIC("Assertion '" << #cond << "' failed: " << error); \
            }                                                           \
        } while (false)

#else

    #define ASSERT(cond, error)                        \
        do {                                           \
            if (false) {                               \
                [[maybe_unused]] bool toIgnore = cond; \
            }                                          \
        } while (false)

#endif

////////////////////////////////////////////////////////////////////////////////

#define VERIFY(cond, error)                                         \
    do {                                                            \
        if (UNLIKELY(!(cond))) { /*NOLINT*/                         \
            PANIC("Assertion '" << #cond << "' failed: " << error); \
        }                                                           \
    } while (false)

#define ENSURE(cond, error)                                                                 \
    do {                                                                                    \
        if (UNLIKELY(!(cond))) { /*NOLINT*/                                                 \
            ::common::util::detail::Throw(CODE_HERE, (std::stringstream() << error).str()); \
        }                                                                                   \
    } while (false)
