#include "runtime.hpp"
#include "config.pb.h"
#include "postboy.pb.h"

#include <postboy/base/binding.hpp>
#include <postboy/base/postboy.hpp>
#include <postboy/prepared/prepared.hpp>
#include <postboy/prometheus_metrics/gateway.hpp>

#include <common/util/assert.hpp>

#include <memory>
#include <stop_token>
#include <thread>

namespace postboy {

Runtime::Runtime(std::shared_ptr<config::Config> config)
    : config_(std::move(config)) {
    ENSURE(config_ != nullptr, "Emtpy config");
}

void Runtime::RunPrepared() {
    postboy::Prepared prepared(config_->prepared());
    prepared.Run();
}

void Runtime::CreatePostboys() {
    for (const auto& postboyConfig : config_->postboy()) {
        auto postboy = std::make_shared<postboy::Postboy>(postboyConfig);
        ENSURE(!postboys_.contains(postboy->Name()), "Postboy \"" << postboy->Name() << "\" already exists");
        postboy->Init();
        postboys_[postboy->Name()] = std::move(postboy);
    }
}

void Runtime::CreateBindings() {
    for (const auto& bindingConfig : config_->binding()) {
        auto binding = SetupBinding(bindingConfig, postboys_);
        ENSURE(!bindings_.contains(binding->Name()), "Binding \"" << binding->Name() << "\" already exists");
        bindings_[binding->Name()] = std::move(binding);
    }
}

void Runtime::CreatePrometheusPushGateway() {
    auto pushGateway = postboy::prometheus::CreatePushGatewayFromConfig(postboy::GlobalRegistry(), *config_);

    if (pushGateway == nullptr) {
        return;
    }
    prometheusPushGatewayThread_.emplace([pushGateway = std::move(pushGateway)](std::stop_token token) {
        while (!token.stop_requested()) {
            pushGateway->Push();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void Runtime::ValidateGraph() {
    for (const auto& [_, postboy] : postboys_) {
        ENSURE(postboy->IsValid(), "Postboy \"" << postboy->Name() << "\" is not valid");
    }
}

void Runtime::Prepare() {
    ENSURE(config_, "Empty postboy config");
    thread_manager::Load({config_->thread().begin(), config_->thread().end()});
    RunPrepared();
    CreatePostboys();
    CreateBindings();
    ValidateGraph();
}

void Runtime::Run() {
    for (const auto& [_, postboy] : postboys_) {
        postboy->Run();
    }
    CreatePrometheusPushGateway();
}

void Runtime::WaitFinished() {
    for (const auto [_, postboy] : postboys_) {
        auto state = postboy->State();
        while (state != PostboyState::Finished) {
            postboy->Wait(state);
        }
    }
}

const config::Config& Runtime::Config() const {
    return *config_;
}

std::optional<std::reference_wrapper<Postboy>> Runtime::Postboy(const std::string& name) {
    auto iter = postboys_.find(name);
    if (iter == postboys_.end()) {
        return std::nullopt;
    }
    return std::ref(*iter->second);
}

std::optional<std::reference_wrapper<Binding>> Runtime::Binding(const std::string& name) {
    auto iter = bindings_.find(name);
    if (iter == bindings_.end()) {
        return std::nullopt;
    }
    return std::ref(*iter->second);
}

void Runtime::CreatePostboy(config::Postboy config) {
    ENSURE(!postboys_.contains(config.name()), "Postboy \"" << config.name() << "\" already exists");
    *config_->add_postboy() = config;
    auto postboy = std::make_shared<postboy::Postboy>(std::move(config));
    postboy->Init();
    postboys_[postboy->Name()] = std::move(postboy);
}

void Runtime::CreateBinding(config::Binding config) {
    ENSURE(!bindings_.contains(config.name()), "Binding \"" << config.name() << "\" already exists");
    *config_->add_binding() = config;
    auto binding = SetupBinding(std::move(config), postboys_);
    bindings_[binding->Name()] = std::move(binding);
}

} // namespace postboy
