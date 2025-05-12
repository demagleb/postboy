#include "binary_dir.hpp"

#include <common/util/assert.hpp>

#include <linux/limits.h>
#include <unistd.h>

namespace common::system {

std::filesystem::path GetCurrentBinaryDir() {
    char pBuf[PATH_MAX];
    size_t len = sizeof(pBuf);
    int bytes = readlink("/proc/self/exe", pBuf, len);
    ENSURE(bytes >= 0, "bytes=[" << bytes << "] is 0");
    pBuf[bytes] = '\0';
    return std::filesystem::path(pBuf).parent_path();
}

} // namespace common::system
