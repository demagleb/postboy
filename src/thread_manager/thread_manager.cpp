#include "thread_manager.hpp"
#include <optional>

#include <errno.h>
#include <pthread.h>
#include <thread>

namespace thread_manager {

#define handle_error_en(en, msg) \
    do {                         \
        errno = en;              \
        perror(msg);             \
        exit(EXIT_FAILURE);      \
    } while (0)

ThreadManager::ThreadManager(const std::vector<config::Thread>& config) {
    INFO("Init thread manager. Config size = " << config.size());

    for (size_t i = 0; i < config.size(); ++i) {
        ParseConfigThread(config[i]);
    }
}

std::optional<types::ThreadModel> ThreadManager::GetModel(std::string_view postBoyName) {
    if (serviceToThreadModel_.contains(postBoyName.data())) {
        return serviceToThreadModel_[postBoyName.data()];
    }
    return std::nullopt;
}

void ThreadManager::Configure(std::string_view name) {
    auto threadModel = GetModel(name);

    if (!threadModel.has_value()) {
        INFO(fmt::format("No thread model for postboy with name {}", name));
        return;
    }

    if (threadModel->coreList.has_value()) {
        SetAffinity(*(threadModel->coreList));
    }
    if (threadModel->nice.has_value()) {
        SetNice(*(threadModel->nice));
    }

    // TODO: priority shows -x - 1 if in config sets to x
    // if (threadModel->priority.has_value()) {
    //     SetPriority(*(threadModel->priority));
    // }
}

void ThreadManager::SetAffinity(types::CpuCoreList coreList) {
    auto threadId = ::pthread_self();

    if (coreList.empty()) {
        return;
    }

    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet);

    for (const auto& cpu : coreList) {
        CPU_SET(cpu, &cpuSet);
    }

    int error = ::pthread_setaffinity_np(threadId, sizeof(cpu_set_t), &cpuSet);
    ENSURE(error == 0, fmt::format("{} Failed to bin thread to coreList: {}", threadId, common::util::Join(coreList)));
}

void ThreadManager::SetPriority(types::ThreadPriority priority) {
    auto threadId = ::pthread_self();

    sched_param stShedParam{.sched_priority = priority.Value()};
    auto error = ::pthread_setschedparam(threadId, SCHED_FIFO, &stShedParam);

    ENSURE(error == 0, fmt::format("{}: pthread_setshedparam error with result: {}", threadId, error));
}

void ThreadManager::SetNice(types::ThreadNice nice) {
    pthread_t threadId = ::pthread_self();

    auto error = ::setpriority(PRIO_PROCESS, 0, nice.Value());
    ENSURE(error == 0, fmt::format("{}: setpriority error with result: {}", threadId, error));
}

void ThreadManager::ParseConfigThread(const config::Thread& conf) {
    types::ThreadModel threadModel;

    if (!conf.corelist().empty()) {
        threadModel.coreList = std::make_optional<types::CpuCoreList>({});

        for (auto i = 0; i < conf.corelist().size(); ++i) {
            threadModel.coreList->push_back(conf.corelist().at(i));
        }
    }

    if (conf.has_priority()) {
        threadModel.priority = std::make_optional<types::ThreadPriority>(conf.priority());
    }

    if (conf.has_nice()) {
        threadModel.nice = std::make_optional<types::ThreadNice>(conf.nice());
    }

    serviceToThreadModel_[conf.postboyname()] = std::move(threadModel);
}

std::unique_ptr<ThreadManager> threadManager;

void Load(const std::vector<config::Thread>& config) {
    threadManager = std::make_unique<ThreadManager>(config);
}

ThreadManager& Get() {
    ASSERT(threadManager != nullptr, "Thread manager is not initialized");
    return *threadManager.get();
}

} // namespace thread_manager
