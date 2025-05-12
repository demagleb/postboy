#pragma once

#include <carry/detail/storage.hpp>

#include <common/util/assert.hpp>

#include <optional>
#include <cstdint>

namespace carry {

//////////////////////////////////////////////////////////////////////

// Immutable mapping from string keys to arbitrary values

class Context {
    using StorageRef = detail::StorageRef;

public:
    // Generic accessors

    template <typename TValue>
    std::optional<TValue> TryGet(const Key& key) const;

    template <typename TValue>
    TValue Get(const Key& key) const;

    template <typename TValue>
    TValue GetOr(const Key& key, TValue orValue) const;

    Keys CollectKeys(std::string_view prefix) const {
        return storage_->CollectKeys(prefix);
    }

    // Common types

    std::string GetString(const Key& key) const {
        return Get<std::string>(key);
    }

    int64_t GetInt64(const Key& key) const {
        return Get<int64_t>(key);
    }

    uint64_t GetUInt64(const Key& key) const {
        return Get<uint64_t>(key);
    }

    bool GetBool(const Key& key) const {
        return Get<bool>(key);
    }

    // For constructors
    static Context FromStorage(StorageRef storage) {
        return Context(std::move(storage));
    }

    Context();

    // Private!
    StorageRef StoragePrivate() {
        return storage_;
    }

private:
    explicit Context(StorageRef storage)
        : storage_(std::move(storage)) {
    }

private:
    StorageRef storage_;
};

template <typename TValue>
std::optional<TValue> Context::TryGet(const Key& key) const {
    auto anyValue = storage_->Lookup(key);

    if (anyValue.has_value()) {
        TValue* typedValue = std::any_cast<TValue>(&anyValue);
        if (typedValue) {
            return *typedValue;
        } else {
            PANIC("Unexpected type for context key '" << key << "'");
        }
    }

    return std::nullopt;
}

template <typename TValue>
TValue Context::Get(const Key& key) const {
    auto maybeValue = TryGet<TValue>(key);
    VERIFY(maybeValue.has_value(), "Key " << key << " is missing");
    return *maybeValue;
}

template <typename TValue>
TValue Context::GetOr(const Key& key, TValue orValue) const {
    return TryGet<TValue>(key).value_or(orValue);
}

} // namespace carry

//////////////////////////////////////////////////////////////////////
