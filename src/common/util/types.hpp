#pragma once

#define FORCE_INLINE inline __attribute__((always_inline))
#define FORCE_INLINE_LAMBDA __attribute__((always_inline))
#define NEVER_INLINE __attribute__((noinline))
#define HOT_SPOT __attribute__((hot, flatten))

#define LIKELY(condition) __builtin_expect(static_cast<bool>(condition), 1)
#define UNLIKELY(condition) __builtin_expect(static_cast<bool>(condition), 0)

#define CONCAT(X, Y) CONCAT_IMPL_I(X, Y)
#define CONCAT_IMPL_I(X, Y) CONCAT_IMPL_II(X, Y)
#define CONCAT_IMPL_II(X, Y) X##Y
