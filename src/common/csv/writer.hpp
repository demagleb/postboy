#pragma once

#include "types.hpp"

#include <string>
#include <vector>
#include <fstream>

namespace common::csv {

class Writer {
public:
    Writer(std::ostream& out, Row headers, char sep = '|');

    void WriteRow(const Row& row);

    void WriteRow(const RowMap& row);

    void Dump();

private:
    std::ostream& out_;
    const char sep_;
    Row headers_;
};

} // namespace common::csv
