#pragma once

#include <config/proto/config.pb.h>

#include <filesystem>

namespace config {

Config FromFS(const std::filesystem::path& path);

static constexpr std::string_view kDefaultConfigPath = "/etc/postboy/config.conf";

void ValidatePath(const std::filesystem::path& path);

} // namespace config
