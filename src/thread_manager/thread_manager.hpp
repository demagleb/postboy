#pragma once

#include <common/util/assert.hpp>
#include <common/util/strings.hpp>
#include <config/proto/thread.pb.h>
#include <thread_manager/types.hpp>

#include <fmt/format.h>
#include <tsl/robin_map.h>

#include <memory>
#include <optional>
#include <string_view>

#include <pthread.h>
#include <sys/resource.h>

namespace thread_manager {

class ThreadManager {
public:
    explicit ThreadManager(const std::vector<config::Thread>& config);

    std::optional<types::ThreadModel> GetModel(std::string_view postBoyName);

    void Configure(std::string_view name);

protected:
    void SetAffinity(types::CpuCoreList coreList);

    void SetPriority(types::ThreadPriority priority);

    void SetNice(types::ThreadNice nice);

    void ParseConfigThread(const config::Thread& conf);

private:
    tsl::robin_map<std::string, types::ThreadModel> serviceToThreadModel_;
};

void Load(const std::vector<config::Thread>& config);

ThreadManager& Get();

} // namespace thread_manager
