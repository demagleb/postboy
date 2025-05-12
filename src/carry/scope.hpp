#pragma once

#include <carry/current.hpp>

#include <carry/wrap.hpp>

namespace carry {

//////////////////////////////////////////////////////////////////////

class Scope {
public:
    // Setup context `target`
    Scope(carry::Context ctx)
        : prev_(Current()) {
        Set(std::move(ctx));
    }

    // Rollback previous context
    ~Scope() {
        Set(prev_);
    }

private:
    void Set(carry::Context context);

private:
    carry::Context prev_;
};

//////////////////////////////////////////////////////////////////////

/*
 * Example:
 *
 * {
 *   // Wrap current task context with key/value pair
 *   context::WrapScope<std::string> scope("key", "value");
 *
 *   auto value = context::Current().Get<std::string>("key");
 *
 * }  // <- Restore previous context
 *
 */

template <typename TValue>
class WrapScope : public Scope {
public:
    WrapScope(std::string key, TValue value)
        : Scope(Wrap(Current()).Set(key, value).Done()) {
    }
};

} // namespace carry
