#pragma once

#include <compass/map.hpp>

namespace compass {

template <typename TService>
TService& Locate() {
    return Map().Locate<TService>();
}

} // namespace compass
