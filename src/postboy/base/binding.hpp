#pragma once

#include "binding.pb.h"
#include <config/config.h>
#include <common/util/assert.hpp>
#include <postboy/base/queues/queue.hpp>
#include <memory>

namespace postboy {

class Postboy;

class Binding {
public:
    virtual ~Binding() = default;

    const std::string& Name() const {
        return config_.name();
    }

    const config::Binding& Config() const {
        return config_;
    }

    virtual void Setup(config::Binding config,
                       const std::unordered_map<std::string, std::shared_ptr<postboy::Postboy>>& postboys);

    virtual void AddProducer(std::shared_ptr<postboy::Postboy> postboy, size_t index) = 0;

    virtual void AddConsumer(std::shared_ptr<postboy::Postboy> postboy, size_t index) = 0;

protected:
    config::Binding config_;
    std::string name_;
};

std::unique_ptr<Binding>
SetupBinding(config::Binding config,
             const std::unordered_map<std::string, std::shared_ptr<postboy::Postboy>>& postboys);

} // namespace postboy
