#pragma once

#include <carry/context.hpp>

#include <carry/detail/entries.hpp>

#include <map>
#include <string_view>

namespace carry {

//////////////////////////////////////////////////////////////////////

/*
 * Example:
 *
 * Wrapper wrapper(ctx1);
 * wrapper.SetString("key", "value");
 * wrapper.SetInt64("timeout", 1000);
 *
 * auto ctx2 = wrapper.Done();
 */

class [[nodiscard]] Wrapper {
public:
    Wrapper(Context wrapped);

    // Non-copyable
    Wrapper(const Wrapper&) = delete;
    Wrapper& operator=(const Wrapper&) = delete;

    // Non-movable
    Wrapper(Wrapper&&) = delete;
    Wrapper& operator=(Wrapper&&) = delete;

    // Generic set

    template <typename T>
    Wrapper& Set(Key key, T value) {
        new_entries_.insert_or_assign(std::move(key), std::move(value));
        return *this;
    }

    // Workaround for Set("key", "value")
    template <>
    Wrapper& Set(Key key, const char* value) {
        return SetString(key, std::string{value});
    }

    // Setters for common types

    Wrapper& SetString(Key key, std::string value) {
        return Set(key, value);
    }

    Wrapper& SetString(Key key, std::string_view value) {
        return SetString(key, std::string{value});
    }

    Wrapper& SetString(Key key, const char* value) {
        return SetString(key, std::string{value});
    }

    Wrapper& SetInt64(Key key, int64_t value) {
        return Set(key, value);
    }

    Wrapper& SetUInt64(Key key, uint64_t value) {
        return Set(key, value);
    }

    Wrapper& SetBool(Key key, bool flag) {
        return Set(key, flag);
    }

    // One-shot
    Context Done();

    // Implicit Done
    operator Context() {
        return Done();
    }

protected:
    detail::StorageRef wrapped_storage_;
    detail::Entries new_entries_;
};

//////////////////////////////////////////////////////////////////////

class ReWrapper : public Wrapper {
public:
    ReWrapper(Context wrapped)
        : Wrapper(std::move(wrapped)) {
    }

    void ReWrap(Context wrapped) {
        wrapped_storage_ = wrapped.StoragePrivate();
    }
};

//////////////////////////////////////////////////////////////////////

/*
 * Example:
 *
 * auto ctx2 = Wrap(ctx1)
 *              .SetString("key", "value")
 *              .SetInt64("timeout", 1000)
 *              .SetBool("flag", true)
 *              .Done();
 */

inline Wrapper Wrap(Context that) {
    return {std::move(that)};
}

} // namespace carry
