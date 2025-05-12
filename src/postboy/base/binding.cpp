#include "binding.hpp"

#include <postboy/base/postboy.hpp>

namespace postboy {

void Binding::Setup(config::Binding config, const std::unordered_map<std::string, std::shared_ptr<postboy::Postboy>>&) {
    config_ = std::move(config);
}

std::unique_ptr<Binding>
SetupBinding(config::Binding config,
             const std::unordered_map<std::string, std::shared_ptr<postboy::Postboy>>& postboys) {
    ENSURE(!config.producer().empty(), "Binding \"" << config.name() << "\": producers empty");
    const auto& someProducer = config.producer(0);
    auto binding = postboys.at(someProducer.postboyname())->Processor().CreateOutputBinding(someProducer.index());
    binding->Setup(std::move(config), postboys);
    return binding;
}

} // namespace postboy
