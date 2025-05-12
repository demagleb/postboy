#pragma once

#include <common/util/assert.hpp>

#include <filesystem>
#include <fstream>

#include <sys/stat.h>
#include <unistd.h>

namespace common::system {

size_t GetFileSize(const std::filesystem::path& fileName);

template <class Result>
Result ReadFile(const std::filesystem::path& path) {
    const auto fileSize = GetFileSize(path);
    if (fileSize == 0) {
        return Result{};
    }
    std::filebuf file;
    ENSURE(file.open(path.c_str(), std::ios_base::in | std::ios_base::binary),
           "Error while opening file " << path.string());
    Result result(fileSize, '\0');
    ENSURE(static_cast<size_t>(file.sgetn(reinterpret_cast<char*>(&result[0]), fileSize)) == fileSize,
           "Error while reading file: " << path.string());
    return result;
}

bool IsFile(const std::filesystem::path& path);

bool IsReadableFile(const std::filesystem::path& path);

} // namespace common::system
