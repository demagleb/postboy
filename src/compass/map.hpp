#pragma once

#include <compass/locator.hpp>

#include <map>
#include <string>
#include <typeinfo>

namespace compass {

//////////////////////////////////////////////////////////////////////

class Map {
public:
    using Key = std::string;

    template <typename TService>
    Map& Add(Locator<TService>& locator);

    template <typename TService>
    TService& Locate();

    template <typename TService>
    TService* TryLocate();

    // For tests
    void Clear() {
        entries_.clear();
    }

private:
    template <typename TService>
    static std::string KeyFor() {
        return typeid(TService).name();
    }

    [[noreturn]] void Fail(std::string_view reason);

private:
    std::map<Key, LocatorBase*> entries_;
};

//////////////////////////////////////////////////////////////////////

// Implementation

template <typename TService>
Map& Map::Add(Locator<TService>& locator) {
    Key key = KeyFor<TService>();
    entries_.insert_or_assign(key, &locator);
    return *this;
}

template <typename TService>
TService& Map::Locate() {
    Key key = KeyFor<TService>();

    auto it = entries_.find(key);

    if (it == entries_.end()) {
        Fail("Service not found");
    }

    Locator<TService>* locator = dynamic_cast<Locator<TService>*>(it->second);
    if (locator == nullptr) {
        Fail("Service type mismatch");
    }

    TService* service = locator->Locate(Tag<TService>{});

    if (service == nullptr) {
        Fail("Service instance not found");
    }

    return *service;
}

template <typename TService>
TService* Map::TryLocate() {
    Key key = KeyFor<TService>();

    auto it = entries_.find(key);

    if (it == entries_.end()) {
        return nullptr;
    }

    Locator<TService>* locator = dynamic_cast<Locator<TService>*>(it->second);

    if (locator == nullptr) {
        Fail("Service type mismatch");
    }

    return locator->Locate(Tag<TService>{});
}

//////////////////////////////////////////////////////////////////////

Map& Map();

} // namespace compass
