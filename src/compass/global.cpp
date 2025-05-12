#include <compass/map.hpp>

namespace compass {

class Map& Map() {
    // TODO: Leaky singleton
    static class Map map;

    return map;
}

} // namespace compass
