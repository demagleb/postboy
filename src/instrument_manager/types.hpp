#pragma once

#include <string>

namespace instrument_manager {

using ProductId = int;

struct Instrument {
    std::string symbol;
    ProductId id;
    double tickSize;
    double stepSize;
    double minPrice;
    double minQty;
    int maxNumOrders;
};

} // namespace instrument_manager
