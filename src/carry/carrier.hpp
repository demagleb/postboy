#pragma once

#include <carry/context.hpp>

namespace carry {

struct ICarrier {
    virtual ~ICarrier() = default;

    virtual void Set(Context context) = 0;
    virtual const Context& GetContext() = 0;
};

} // namespace carry
