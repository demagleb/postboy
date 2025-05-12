#include "source_location.hpp"

namespace common::util {

// Format: {file}:{function}[Line {line_number}]

std::ostream& operator<<(std::ostream& out, const SourceLocation& where) {
    out << where.File() << ":" << where.Function() << "[Line " << where.Line() << "]";
    return out;
}

} // namespace common::util
