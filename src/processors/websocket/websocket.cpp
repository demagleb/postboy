#include <common/util/assert.hpp>
#include <postboy/base/metrics.hpp>
#include <postboy/base/putable_processor.hpp>
#include <postboy/base/registry.hpp>
#include <postboy/prometheus_metrics/counter.hpp>
#include <processors/websocket/metrics.hpp>
#include <websocket/client.hpp>

#include <ev.h>
#include <fmt/format.h>

#include <memory>

namespace postboy {

namespace {

class Websocket : public PutableProcessor<std::string> {
public:
    Websocket()
        : PutableProcessor<std::string>(1) {
    }

    void Process() override {
        ev_run(evLoop_, 0);
    }

    void Init(const config::Process& config) override {
        ASSERT(config.Processes_case() == config::Process::kWebsocket,
               fmt::format("Process case for [Websocker] from config is not [kWebsocker]"));
        INFO("Init Websocket");
        metrics_.receivePacketCounter_->Reset();
        const auto& c = config.websocket();
        evLoop_ = ev_loop_new();
        client_ = std::make_unique<websocket::Client>(
            evLoop_, [this](std::string&& s) { Websocket::OnReceive(std::move(s)); }, Websocket::OnChangeState);
        client_->Connect(c.host(), c.path(), c.port());
    }

    ~Websocket() override {
        client_->Finish();
        if (evLoop_ != nullptr) {
            ev_loop_destroy(evLoop_);
        }
        metrics_.receivePacketCounter_->Reset();
    }

    void InitMetrics(const std::string& name) override {
        metrics_.InitMetrics()(*registry_, name);
    }

private:
    void OnReceive(std::string&& s) {
        Put(std::move(s));
        metrics_.receivePacketCounter_->Increment();
    }

    static void OnChangeState(websocket::Client::State state, std::string&& reason) {
        INFO(reason);
    }

private:
    std::unique_ptr<websocket::Client> client_;
    struct ev_loop* evLoop_;

    // Metrics
    WebsocketMetrics metrics_;
};

} // namespace

REGISTER_PROCESSOR(Websocket, config::Process::ProcessesCase::kWebsocket);

} // namespace postboy
