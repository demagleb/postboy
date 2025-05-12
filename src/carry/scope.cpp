#include <carry/scope.hpp>

#include <carry/carrier.hpp>

#include <compass/locate.hpp>

namespace carry {

void Scope::Set(carry::Context context) {
    compass::Locate<ICarrier>().Set(std::move(context));
}

} // namespace carry
