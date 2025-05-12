#include "config.h"

#include <google/protobuf/text_format.h>
#include <fmt/format.h>

#include <common/system/file.hpp>
#include <common/util/assert.hpp>

using namespace common::system;

namespace config {

Config FromFS(const std::filesystem::path& path) {
    Config config;
    const auto protoString = ReadFile<std::string>(path);
    ENSURE(google::protobuf::TextFormat::ParseFromString(protoString, &config),
           fmt::format("Could not parse config file [{}]", path.c_str()));
    return config;
}

void ValidatePath(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        if (path.string() == kDefaultConfigPath) {
            throw std::runtime_error(fmt::format("Default path [{}] does not exist. Create file by default path [{}] "
                                                 "or implicity specify the path to the config via -c/--config argument",
                                                 kDefaultConfigPath, kDefaultConfigPath));
        }
        throw std::runtime_error(
            fmt::format("Path [{}] does not exist. Specify another path to the config", path.string()));
    }
    if (!common::system::IsReadableFile(path)) {
        throw std::runtime_error(fmt::format("Path [{}] is not the readable file", path.string()));
    }
}

} // namespace config
