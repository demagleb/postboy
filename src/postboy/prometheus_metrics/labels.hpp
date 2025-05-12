#pragma once

#include <tsl/robin_map.h>

namespace postboy::prometheus {

using Labels = tsl::robin_map<std::string, std::string>;

} // namespace postboy::prometheus
