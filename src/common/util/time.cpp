#include "time.hpp"

namespace common::util {

std::string GetCurrentDate() {
    std::array<char, 64> buffer;
    buffer.fill(0);
    time_t rawtime;
    time(&rawtime);
    strftime(buffer.data(), sizeof(buffer), "%Y_%m_%d", localtime(&rawtime));
    return buffer.data();
}

} // namespace common::util
