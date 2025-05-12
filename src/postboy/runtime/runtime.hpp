#pragma once

#include "binding.pb.h"
#include "config.pb.h"
#include "postboy.pb.h"
#include "process.pb.h"
#include <functional>
#include <postboy/base/binding.hpp>
#include <postboy/base/postboy.hpp>
#include <config/config.h>

#include <memory>
#include <thread>
#include <unordered_map>

namespace postboy {

class Runtime {
public:
    explicit Runtime(std::shared_ptr<config::Config> config);

    void Prepare();
    void Run();

    void WaitFinished();

    const config::Config& Config() const;

    std::optional<std::reference_wrapper<Postboy>> Postboy(const std::string& name);
    std::optional<std::reference_wrapper<Binding>> Binding(const std::string& name);

    void CreatePostboy(config::Postboy config);
    bool RemovePostboy(const std::string& name);

    void CreateBinding(config::Binding config);
    bool RemoveBinding(const std::string& name);

private:
    void RunPrepared();
    void CreatePostboys();
    void CreateBindings();
    void CreatePrometheusPushGateway();
    void ValidateGraph();

    std::shared_ptr<config::Config> config_;
    std::unordered_map<std::string, std::shared_ptr<postboy::Postboy>> postboys_;
    std::vector<std::thread> postboyThreads_;
    std::optional<std::jthread> prometheusPushGatewayThread_;
    std::unordered_map<std::string, std::unique_ptr<postboy::Binding>> bindings_;
};

} // namespace postboy
