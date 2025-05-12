#pragma once

#include <string>
#include <any>

#include <set>

namespace carry {

using Key = std::string;
using Value = std::any; // includes "missing" state

using Keys = std::set<Key>;

} // namespace carry
