#include "writer.hpp"

namespace common::csv {

Writer::Writer(std::ostream& out, Row headers, char sep)
    : out_(out)
    , sep_(sep)
    , headers_(headers) {
}

void Writer::WriteRow(const Row& row) {
    out_ << row[0];
    for (int i = 1; i < static_cast<int>(row.size()); ++i) {
        out_ << sep_ << row[i];
    }
    out_ << "\n";
}

void Writer::WriteRow(const RowMap& row) {
    out_ << row.at(headers_[0]);

    for (int i = 1; i < static_cast<int>(row.size()); ++i) {
        out_ << sep_ << row.at(headers_[i]);
    }
    out_ << "\n";
}

void Writer::Dump() {
    out_.flush();
}

} // namespace common::csv
