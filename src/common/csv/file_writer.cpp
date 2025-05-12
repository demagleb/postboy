#include "file_writer.hpp"

#include <common/csv/writer.hpp>

namespace common::csv {

FileWriter::FileWriter(const std::string& filename, Row headers)
    : Writer(out_, headers)
    , out_(filename) {
    Writer::WriteRow(headers);
}

void FileWriter::WriteRow(const Row& row) {
    Writer::WriteRow(row);
}

void FileWriter::WriteRow(const RowMap& row) {
    Writer::WriteRow(row);
}

void FileWriter::Dump() {
    Writer::Dump();
}

} // namespace common::csv
