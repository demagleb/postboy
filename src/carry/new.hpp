#pragma once

#include <carry/empty.hpp>
#include <carry/wrap.hpp>

namespace carry {

//////////////////////////////////////////////////////////////////////

/*
 * Example:
 *
 * Builder builder;
 * builder.SetString("key", "value");
 * builder.SetInt64("timeout", 1000);
 *
 * auto ctx = builder.Done();
 */

class Builder : public Wrapper {
public:
    Builder()
        : Wrapper(Empty()) {
    }
};

//////////////////////////////////////////////////////////////////////

/*
 * Example:
 *
 * auto ctx = context::New()
 *              .SetString("key", "value")
 *              .SetInt64("timeout", 1000)
 *              .SetBool("flag", true)
 *              .Done();
 */

inline auto New() {
    return Wrap(Empty());
}

} // namespace carry
