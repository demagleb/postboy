#include "instrument_manager.hpp"

#include <common/csv/reader.hpp>
#include <common/util/assert.hpp>
#include <common/util/defer.hpp>

#include <tsl/robin_map.h>

#include <atomic>

namespace instrument_manager {

InstrumentManager::InstrumentManager(const std::string& filename) {
    std::ifstream inp(filename);
    defer {
        inp.close();
    };
    common::csv::Reader reader(inp, headers_);

    INFO(filename);

    for (std::optional<common::csv::RowMap> row; (row = reader.ReadMap()) != std::nullopt;) {
        Instrument instrument{.symbol = (*row)["symbol"],
                              .id = newProductId_,
                              .tickSize = std::stod((*row)["tickSize"]),
                              .stepSize = std::stod((*row)["stepSize"]),
                              .minQty = std::stod((*row)["minQty"]),
                              .maxNumOrders = std::stoi((*row)["maxNumOrders"])};

        idToInstrument_.emplace(newProductId_++, instrument);
        symbolToInstrument_.emplace(instrument.symbol, instrument);
    }
}

std::optional<Instrument> InstrumentManager::GetInstrument(ProductId id) {
    if (idToInstrument_.contains(id)) {
        return idToInstrument_[id];
    }
    return std::nullopt;
}

std::optional<Instrument> InstrumentManager::GetInstrument(std::string_view symbol) {
    if (symbolToInstrument_.contains(symbol.data())) {
        return symbolToInstrument_[symbol.data()];
    }
    return std::nullopt;
}

namespace {

struct Status {
    enum : int { Init = 0, Running = 1, Finished = 2 };
};

std::atomic<int> status{Status::Init};
std::unique_ptr<InstrumentManager> instrumentManager;

} // namespace

void Load(const std::string& filename) {
    int expected = Status::Init;
    if (status.compare_exchange_strong(expected, Status::Running)) {
        instrumentManager = std::make_unique<InstrumentManager>(filename);
        status.store(Status::Finished);
    } else {
        if (expected == Status::Finished) {
            return;
        }
        while (status.load() != Status::Finished) {
        }
    }
}

InstrumentManager& Get() {
    ASSERT(instrumentManager != nullptr, "Instrument manager is not initialized");
    return *instrumentManager.get();
}

} // namespace instrument_manager
