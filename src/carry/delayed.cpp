#include <carry/delayed.hpp>

#include <carry/empty.hpp>

namespace carry {

namespace detail {

DelayedStorage::DelayedStorage()
    : storage_(Empty().StoragePrivate()) {
    //
}

} // namespace detail

DelayedContext::DelayedContext()
    : delayed_(refer::New<detail::DelayedStorage>()) {
}

Context DelayedContext::Make() const {
    return Context::FromStorage(delayed_);
}

void DelayedContext::Install(Context context) {
    delayed_->Install(context.StoragePrivate());
}

} // namespace carry
