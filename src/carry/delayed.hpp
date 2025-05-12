#pragma once

#include <carry/context.hpp>

#include <carry/detail/storage.hpp>

#include <refer/ref_counted.hpp>

namespace carry {

namespace detail {

//////////////////////////////////////////////////////////////////////

class DelayedStorage : public IStorage, public refer::RefCounted<DelayedStorage> {
public:
    DelayedStorage();

    // IStorage

    Value Lookup(const Key& key) override {
        return storage_->Lookup(key);
    }
    Keys CollectKeys(std::string_view prefix) override {
        return storage_->CollectKeys(prefix);
    }

    void Install(StorageRef storage) {
        storage_ = std::move(storage);
    }

private:
    StorageRef storage_;
};

} // namespace detail

//////////////////////////////////////////////////////////////////////

// Workaround (?) for FlatMap

class DelayedContext {
public:
    DelayedContext();

    // Make empty context
    Context Make() const;

    // Install actual storage
    void Install(Context context);

private:
    refer::Ref<detail::DelayedStorage> delayed_;
};

} // namespace carry
