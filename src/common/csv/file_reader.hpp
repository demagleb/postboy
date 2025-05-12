#pragma once

#include <common/csv/reader.hpp>

#include <fstream>

namespace common::csv {

class FileReader : public Reader {
public:
    FileReader(const std::string& filename, Row headers, bool withHeader);

    std::optional<Row>& Read();
    std::optional<RowMap>& ReadMap();

private:
    std::ifstream inp_;
};

} // namespace common::csv
