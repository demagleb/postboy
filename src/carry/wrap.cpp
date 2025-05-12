#include <carry/wrap.hpp>

#include <refer/ref_counted.hpp>

namespace carry {

namespace {

class WrapStorage : public detail::IStorage, public refer::RefCounted<WrapStorage> {
public:
    WrapStorage(detail::StorageRef wrapped, detail::Entries new_entries)
        : wrapped_storage_(std::move(wrapped))
        , new_entries_(std::move(new_entries)) {
    }

    Value Lookup(const Key& key) override {
        if (auto it = new_entries_.find(key); it != new_entries_.end()) {
            return it->second;
        }
        return wrapped_storage_->Lookup(key);
    }

    Keys CollectKeys(std::string_view prefix) override {
        // Wrapped
        auto keys = wrapped_storage_->CollectKeys(prefix);

        // New
        for (auto [key, _] : new_entries_) {
            if (key.starts_with(prefix)) {
                keys.insert(key);
            }
        }

        return keys;
    }

private:
    detail::StorageRef wrapped_storage_;
    detail::Entries new_entries_;
};

} // namespace

Wrapper::Wrapper(Context wrapped)
    : wrapped_storage_(wrapped.StoragePrivate()) {
}

Context Wrapper::Done() {
    if (new_entries_.empty()) {
        return Context::FromStorage(std::move(wrapped_storage_));
    }

    return Context::FromStorage(refer::New<WrapStorage>(std::move(wrapped_storage_), std::move(new_entries_)));
}

} // namespace carry
