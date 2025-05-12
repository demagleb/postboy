#include "reader.hpp"

#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>

namespace common::csv {

Reader::Reader(std::istream& inp, const Row& headers, bool withHeader)
    : inp_(inp)
    , headers_(headers)
    , withHeader_(withHeader) {
}

std::optional<Row>& Reader::Read() {
    if (withHeader_) {
        Readline();
        withHeader_ = false;
    }

    auto line = Readline();
    if (!line.has_value()) {
        result_ = std::nullopt;
        return result_;
    }

    boost::tokenizer<boost::char_separator<char>> tokens(line.value(), sep_);
    Row result;
    for (auto tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt) {
        result.push_back(*tokenIt);
    }
    result_.emplace(result);

    return result_;
}

std::optional<RowMap>& Reader::ReadMap() {
    if (withHeader_) {
        Readline();
        withHeader_ = false;
    }

    auto line = Readline();
    if (!line.has_value()) {
        resultMap_ = std::nullopt;
        return resultMap_;
    }

    boost::tokenizer<boost::char_separator<char>> tokens(line.value(), sep_);
    RowMap resultMap;
    int idx = 0;
    for (auto tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt) {
        resultMap[headers_[idx]] = *tokenIt;
        ++idx;
    }
    resultMap_.emplace(resultMap);

    return resultMap_;
}

std::optional<std::string> Reader::Readline() {
    if (std::string line; std::getline(inp_, line)) {
        boost::algorithm::trim(line);
        return line.empty() ? std::optional<std::string>() : line;
    }
    return std::nullopt;
}

} // namespace common::csv
