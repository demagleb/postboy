#pragma once

#include <compass/service.hpp>

namespace compass {

//////////////////////////////////////////////////////////////////////

struct LocatorBase {
    virtual ~LocatorBase() = default;
};

//////////////////////////////////////////////////////////////////////

template <typename TService>
struct Tag {};

//////////////////////////////////////////////////////////////////////

template <typename TService>
struct Locator : virtual LocatorBase {
    virtual TService* Locate(Tag<TService>) = 0;
};

} // namespace compass
