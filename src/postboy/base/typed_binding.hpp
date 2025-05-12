#pragma once

#include "binding.pb.h"
#include "common/util/panic.hpp"
#include "postboy/base/queues/mpmc.hpp"
#include "postboy/base/queues/queue_base.hpp"
#include <postboy/base/binding.hpp>
#include <postboy/base/queues/queue.hpp>
#include <postboy/base/postboy.hpp>

#include <common/util/assert.hpp>
#include <memory>
#include <unordered_map>

namespace postboy {

template <typename T>
class TypedBinding : public Binding {
public:
    virtual ~TypedBinding() = default;

    virtual void Setup(config::Binding config,
                       const std::unordered_map<std::string, std::shared_ptr<postboy::Postboy>>& postboys) final {
        for (const auto& producer : config.producer()) {
            ENSURE(postboys.contains(producer.postboyname()), "No such postboy \"" << producer.postboyname() << "\"");
            size_t index = producer.has_index() ? producer.index() : 0;
            producers_.emplace_back(postboys.at(producer.postboyname()), index);
        }
        for (const auto& consumer : config.consumer()) {
            ENSURE(postboys.contains(consumer.postboyname()), "No such postboy \"" << consumer.postboyname() << "\"");
            size_t index = consumer.has_index() ? consumer.index() : 0;
            consumers_.emplace_back(postboys.at(consumer.postboyname()), index);
        }
        if (config.type() == config::BindingType::SPSC) {
            SetupSPSC();
        } else if (config.type() == config::BindingType::SPMC) {
            SetupSPMC();
        } else if (config.type() == config::BindingType::MPSC) {
            SetupMPSC();
        } else if (config.type() == config::BindingType::MPMC) {
            SetupMPMC();
        } else {
            PANIC("Unknown bindingType");
        }
        Binding::Setup(std::move(config), postboys);
    }

    virtual void AddProducer(std::shared_ptr<postboy::Postboy> postboy, size_t index) final {
        if (config_.type() != config::BindingType::MPMC && config_.type() != config::BindingType::MPSC) {
            PANIC("Try to add producer to Single-Producer binding");
        }
        postboy->Processor().LinkOutputQueue(index, queue_);

        auto producer = config_.add_producer();
        producer->set_postboyname(postboy->Name());
        producer->set_index(index);

        producers_.emplace_back(postboy, index);
    }

    virtual void AddConsumer(std::shared_ptr<postboy::Postboy> postboy, size_t index) final {
        if (config_.type() != config::BindingType::MPMC && config_.type() != config::BindingType::SPMC) {
            PANIC("Try to add producer to Single-Consumer binding");
        }
        postboy->Processor().LinkInputQueue(index, queue_);

        auto consumer = config_.add_consumer();
        consumer->set_postboyname(postboy->Name());
        consumer->set_index(index);

        consumers_.emplace_back(postboy, index);
    }

private:
    void SetupSPSC() {
        auto queue = std::make_shared<SPSCQueue<T>>();
        auto producer = producers_.front().first.lock();
        producer->Processor().LinkOutputQueue(producers_.front().second, queue);
        auto consumer = consumers_.front().first.lock();
        consumer->Processor().LinkInputQueue(consumers_.front().second, queue);
    }

    void SetupSPMC() {
        SetupMPMC();
    }

    void SetupMPSC() {
        SetupMPMC();
    }

    void SetupMPMC() {
        auto queue = std::make_shared<MPMCQueue<T>>();
        for (const auto& [weakPostboy, index] : producers_) {
            auto postboy = weakPostboy.lock();
            postboy->Processor().LinkOutputQueue(index, queue);
        }
        for (const auto& [weakPostboy, index] : consumers_) {
            auto postboy = weakPostboy.lock();
            postboy->Processor().LinkInputQueue(index, queue);
        }
    }

    std::shared_ptr<IQueueBase> queue_;
    std::vector<std::pair<std::weak_ptr<postboy::Postboy>, size_t>> producers_;
    std::vector<std::pair<std::weak_ptr<postboy::Postboy>, size_t>> consumers_;
};

} // namespace postboy
