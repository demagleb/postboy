#pragma once

#include <config/proto/config.pb.h>

#include <postboy/prometheus_metrics/collectable.hpp>
#include <postboy/prometheus_metrics/labels.hpp>

#include <chrono>
#include <memory>
#include <string>

namespace postboy::prometheus {

class PushGateway {
public:
    PushGateway(Collectable* collectable, const std::string& host, const std::string& port, const std::string& jobname,
                const Labels& labels = {}, const std::string& username = {}, const std::string& password = {});

    PushGateway(const PushGateway&) = delete;
    PushGateway(PushGateway&&) = delete;
    PushGateway& operator=(const PushGateway&) = delete;
    PushGateway& operator=(PushGateway&&) = delete;

    ~PushGateway();

    int Push();

    int PushAdd();

private:
    enum class HttpMethod {
        POST = 0,
        PUT = 1,
        DELETE = 2,
    };

    class CurlWrapper;

private:
    std::string GetUri() const;

    int PushInternal(HttpMethod method);

private:
    Collectable* collectable_{nullptr};

    std::string jobUri_;
    std::string labels_;
    std::string auth_;
    std::unique_ptr<CurlWrapper> curlWrapper_;
};

std::unique_ptr<PushGateway> CreatePushGatewayFromConfig(Collectable* collectable, const config::Config& conf);

void PushWithTimeout(std::unique_ptr<PushGateway> pushGateway, std::chrono::microseconds timeout);

} // namespace postboy::prometheus
