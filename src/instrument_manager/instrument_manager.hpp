#pragma once

#include <instrument_manager/types.hpp>
#include <common/csv/types.hpp>

#include <atomic>
#include <optional>
#include <string>

namespace instrument_manager {

class InstrumentManager {
public:
    explicit InstrumentManager(const std::string& filename);

    std::optional<Instrument> GetInstrument(ProductId id);

    std::optional<Instrument> GetInstrument(std::string_view symbol);

private:
    const common::csv::Row headers_ = {"baseAsset", "quoteAsset", "symbol", "tickSize",
                                       "stepSize",  "minPrice",   "minQty", "maxNumOrders"};

    tsl::robin_map<ProductId, Instrument> idToInstrument_;
    tsl::robin_map<std::string, Instrument> symbolToInstrument_;
    ProductId newProductId_{0};
};

void Load(const std::string& filename);

InstrumentManager& Get();

} // namespace instrument_manager
