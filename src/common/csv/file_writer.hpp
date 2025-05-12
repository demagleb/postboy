#pragma once

#include <common/csv/writer.hpp>

#include <fstream>

namespace common::csv {

class FileWriter : public Writer {
public:
    FileWriter(const std::string& filename, Row headers);

    void WriteRow(const Row& row);

    void WriteRow(const RowMap& row);

    void Dump();

private:
    std::ofstream out_;
};

} // namespace common::csv
