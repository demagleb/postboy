#include <carry/empty.hpp>

#include <carry/detail/storage.hpp>

#include <refer/unmanaged.hpp>

namespace carry {

//////////////////////////////////////////////////////////////////////

struct EmptyStorage : detail::IStorage, refer::Unmanaged {
    Value Lookup(const Key& /*key*/) override {
        return std::any{}; // No value
    }

    Keys CollectKeys(std::string_view /*prefix*/) override {
        return {}; // Empty
    }
};

//////////////////////////////////////////////////////////////////////

static EmptyStorage empty;

// Allocation-free

Context Empty() {
    return Context::FromStorage(refer::Adopt(&empty));
}

} // namespace carry
