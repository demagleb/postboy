#include <carry/current.hpp>

#include <carry/carrier.hpp>

#include <compass/locate.hpp>

namespace carry {

Context Current() {
    return compass::Locate<ICarrier>().GetContext();
}

} // namespace carry
