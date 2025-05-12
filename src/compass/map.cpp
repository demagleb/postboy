#include "map.hpp"

#include <iostream>
#include <cstdlib>

namespace compass {

void Map::Fail(std::string_view reason) {
    std::cerr << "Fail: " << reason << std::endl;
    std::abort();
}

} // namespace compass
