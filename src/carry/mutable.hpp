#pragma once

#include <carry/context.hpp>

#include <carry/detail/entries.hpp>

#include <refer/ref_counted.hpp>

namespace carry {

//////////////////////////////////////////////////////////////////////

namespace detail {

class MutableStorage : public IStorage, public refer::RefCounted<MutableStorage> {
public:
    MutableStorage(Context wrapped);

    // IStorage
    Value Lookup(const Key& key) override;
    Keys CollectKeys(std::string_view prefix) override;

    // Mutation
    void Set(Key key, Value value);

private:
    StorageRef wrapped_;
    Entries mut_entries_;
};

} // namespace detail

//////////////////////////////////////////////////////////////////////

// Set-s should be externally ordered with Lookup-s

struct MutableContext {
    using Mutator = MutableContext;

public:
    MutableContext();

    MutableContext(Context wrapped);

    class Context View();

    template <typename T>
    Mutator& Set(Key key, T value) {
        mut_storage_->Set(key, value);
        return *this;
    }

    Mutator& SetString(Key key, std::string value) {
        return Set(key, value);
    }

    Mutator& SetInt64(Key key, int64_t value) {
        return Set(key, value);
    }

    Mutator& SetUInt64(Key key, uint64_t value) {
        return Set(key, value);
    }

    Mutator& SetBool(Key key, bool flag) {
        return Set(key, flag);
    }

private:
    refer::Ref<detail::MutableStorage> mut_storage_;
};

//////////////////////////////////////////////////////////////////////

// Mutable wrapper for immutable `wrapped`
MutableContext MutableWrap(Context wrapped);

// Empty mutable context
// Synonym for MutableWrap(Empty())
MutableContext NewMutable();

//////////////////////////////////////////////////////////////////////

} // namespace carry
