#include "metrics.hpp"

#include <postboy/base/metrics.hpp>
#include <postboy/prometheus_metrics/registry.hpp>

namespace postboy {

WebsocketMetrics::WebsocketMetrics()
    : IMetricInitializer() {
    initMetrics_ = [this](prometheus::Registry& registry, const std::string& name) -> void {
        auto& counterBuilder = postboy::prometheus::BuildCounter()
                                   .Name(name + "_FileWriterCounter")
                                   .Help("FileWriter proccessor counter")
                                   .Register(registry);

        auto& counter = counterBuilder.Add({{"Processor", "FileWriter"}});
        receivePacketCounter_ = &counter;
    };
}

} // namespace postboy
