#include "file_reader.hpp"

#include <common/csv/reader.hpp>
#include <fstream>

namespace common::csv {

FileReader::FileReader(const std::string& filename, Row headers, bool withHeader)
    : inp_(filename)
    , Reader(inp_, headers, withHeader) {
}

std::optional<Row>& FileReader::Read() {
    return Reader::Read();
}

std::optional<RowMap>& FileReader::ReadMap() {
    return Reader::ReadMap();
}

} // namespace common::csv
