#include <carry/mutable.hpp>

#include <carry/empty.hpp>

namespace carry {

//////////////////////////////////////////////////////////////////////

namespace detail {

MutableStorage::MutableStorage(Context wrapped)
    : wrapped_(wrapped.StoragePrivate()) {
}

Value MutableStorage::Lookup(const Key& key) {
    if (auto it = mut_entries_.find(key); it != mut_entries_.end()) {
        return it->second;
    } else {
        return wrapped_->Lookup(key);
    }
}

Keys MutableStorage::CollectKeys(std::string_view prefix) {
    Keys keys = wrapped_->CollectKeys(prefix);

    for (const auto& [key, _] : mut_entries_) {
        if (key.starts_with(prefix)) {
            keys.insert(key);
        }
    }

    return keys;
}

void MutableStorage::Set(Key key, Value value) {
    mut_entries_.insert_or_assign(std::move(key), std::move(value));
}

} // namespace detail

//////////////////////////////////////////////////////////////////////

MutableContext::MutableContext()
    : MutableContext(Empty()) {
}

MutableContext::MutableContext(Context wrapped)
    : mut_storage_(refer::New<detail::MutableStorage>(std::move(wrapped))) {
}

class Context MutableContext::View() {
    return Context::FromStorage(mut_storage_);
}

//////////////////////////////////////////////////////////////////////

MutableContext MutableWrap(Context context) {
    return {std::move(context)};
}

MutableContext NewMutable() {
    return {};
}

} // namespace carry
