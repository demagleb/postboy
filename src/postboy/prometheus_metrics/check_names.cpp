#include "check_names.hpp"

#include <algorithm>
#include <iterator>

namespace postboy::prometheus {

namespace {

bool IsLocaleIndependentAlphaNumeric(char c) {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool IsLocaleIndependentDigit(char c) {
    return '0' <= c && c <= '9';
}

bool NameStartsValid(const std::string& name) {
    if (name.empty()) {
        return false;
    }

    if (IsLocaleIndependentDigit(name.front())) {
        return false;
    }

    return !(name.compare(0, 2, "__") == 0);
}

} // anonymous namespace

bool CheckMetricName(const std::string& name) {
    if (!NameStartsValid(name)) {
        return false;
    }

    auto validMetricCharacters = [](char c) { return IsLocaleIndependentAlphaNumeric(c) || c == '_' || c == ':'; };

    auto mismatch = std::find_if_not(std::begin(name), std::end(name), validMetricCharacters);

    return mismatch == std::end(name);
}

bool CheckLabelName(const std::string& name, MetricType type) {
    if (!NameStartsValid(name)) {
        return false;
    }

    auto validLabelCharacters = [](char c) { return IsLocaleIndependentAlphaNumeric(c) || c == '_'; };

    if (type == MetricType::Unexpected) {
        return false;
    }

    auto mismatch = std::find_if_not(std::begin(name), std::end(name), validLabelCharacters);

    return mismatch == std::end(name);
}

} // namespace postboy::prometheus
