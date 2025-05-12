#include "file.hpp"

namespace common::system {

size_t GetFileSize(const std::filesystem::path& fileName) {
    std::error_code ec{};
    const auto size = std::filesystem::file_size(fileName, ec);
    ENSURE(!ec, "Cannot get size of the " << fileName << ": " << ec);
    return static_cast<size_t>(size);
}

bool IsFile(const std::filesystem::path& path) {
    struct stat sb;
    return stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode);
}

bool IsReadableFile(const std::filesystem::path& path) {
    return IsFile(path) && access(path.c_str(), R_OK) >= 0;
}

} // namespace common::system
