#pragma once

#include "types.hpp"

#include <boost/tokenizer.hpp>

#include <fstream>
#include <optional>
#include <string>

namespace common::csv {

class Reader {
public:
    using SplitSymbols = boost::char_separator<char>;

    Reader(std::istream& inp, const Row& headers, bool withHeader = true);

    std::optional<Row>& Read();
    std::optional<RowMap>& ReadMap();

private:
    std::optional<std::string> Readline();

private:
    std::istream& inp_;
    Row headers_;
    const SplitSymbols sep_{"|"};
    std::optional<Row> result_;
    std::optional<RowMap> resultMap_;
    bool withHeader_{false};
};

} // namespace common::csv
