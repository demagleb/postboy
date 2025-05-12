#pragma once

#include <carry/kv.hpp>

#include <refer/ref.hpp>

namespace carry {

namespace detail {

//////////////////////////////////////////////////////////////////////

struct IStorage : virtual refer::IManaged {
    virtual ~IStorage() = default;

    virtual Value Lookup(const Key& key) = 0;
    virtual Keys CollectKeys(std::string_view prefix) = 0;
};

//////////////////////////////////////////////////////////////////////

using StorageRef = refer::Ref<IStorage>;

} // namespace detail

} // namespace carry
