#pragma once

#include <string>
#include <vector>

#include <tsl/robin_map.h>

namespace common::csv {

using Row = std::vector<std::string>;
using RowMap = tsl::robin_map<std::string, std::string>;

} // namespace common::csv
