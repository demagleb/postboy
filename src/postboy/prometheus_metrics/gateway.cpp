#include "gateway.hpp"
#include "text_serializer.hpp"

#include <postboy/prometheus_metrics/collectable.hpp>
#include <postboy/prometheus_metrics/labels.hpp>

#include <common/util/assert.hpp>

#include <curl/curl.h>

#include <cstddef>
#include <memory>
#include <sstream>
#include <thread>

namespace postboy::prometheus {

static const char kContentType[] = "Content-Type: text/plain; version=0.0.4; charset=utf-8";

class PushGateway::CurlWrapper {
public:
    CurlWrapper(const std::string& username, const std::string& password) {
        curl_global_init(CURL_GLOBAL_ALL);
        curl_ = curl_easy_init();

        VERIFY(curl_ != nullptr, "Cannot initialize easy curl");

        optHttpHeader_ = curl_slist_append(nullptr, kContentType);

        VERIFY(optHttpHeader_ != nullptr, "Cannot append the header of the content type");

        if (!username.empty()) {
            auth_ = username + ":" + password;
        }
    }

    ~CurlWrapper() {
        curl_slist_free_all(optHttpHeader_);
        curl_easy_cleanup(curl_);
        curl_global_cleanup();
    }

    CurlWrapper(const CurlWrapper&) = delete;
    CurlWrapper(CurlWrapper&&) = delete;
    CurlWrapper& operator=(const CurlWrapper&) = delete;
    CurlWrapper& operator=(CurlWrapper&&) = delete;

    int PerformHttpRequest(HttpMethod method, const std::string& uri, const std::string& body) {
        curl_easy_reset(curl_);
        curl_easy_setopt(curl_, CURLOPT_URL, uri.c_str());

        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, optHttpHeader_);

        if (!body.empty()) {
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, body.size());
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, body.data());
        } else {
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, 0L);
        }

        if (!auth_.empty()) {
            curl_easy_setopt(curl_, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curl_, CURLOPT_USERPWD, auth_.c_str());
        }

        switch (method) {
            case HttpMethod::POST:
                curl_easy_setopt(curl_, CURLOPT_POST, 1L);
                break;

            case HttpMethod::PUT:
                curl_easy_setopt(curl_, CURLOPT_NOBODY, 0L);
                curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PUT");
                break;

            case HttpMethod::DELETE:
                curl_easy_setopt(curl_, CURLOPT_HTTPGET, 0L);
                curl_easy_setopt(curl_, CURLOPT_NOBODY, 0L);
                curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE");
                break;
        }

        auto curlError = curl_easy_perform(curl_);

        int64_t responseCode;
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &responseCode);

        if (UNLIKELY(curlError != CURLE_OK)) {
            return -curlError;
        }

        return responseCode;
    }

    CURL* Curl() {
        if (curl_ == nullptr) {
            curl_ = curl_easy_init();
        }
        return curl_;
    }

private:
    CURL* curl_ = nullptr;
    std::string auth_;
    curl_slist* optHttpHeader_;
};

PushGateway::PushGateway(Collectable* collectable, const std::string& host, const std::string& port,
                         const std::string& jobname, const Labels& labels, const std::string& username,
                         const std::string& password)
    : collectable_(collectable) {
    curl_global_init(CURL_GLOBAL_ALL);
    curlWrapper_ = std::make_unique<CurlWrapper>(username, password);

    std::stringstream jobUriStream;
    jobUriStream << host << ':' << port << "/metrics/job/" << jobname;
    jobUri_ = jobUriStream.str();

    if (!username.empty()) {
        auth_ = username + ":" + password;
    }

    std::stringstream labelStream;
    for (auto& label : labels) {
        labelStream << "/" << label.first << "/" << label.second;
    }
    labels_ = labelStream.str();
}

PushGateway::~PushGateway() {
    curl_global_cleanup();
}

int PushGateway::Push() {
    return PushInternal(HttpMethod::POST);
}

int PushGateway::PushAdd() {
    return PushInternal(HttpMethod::PUT);
}

std::string PushGateway::GetUri() const {
    std::stringstream uri;
    uri << jobUri_ << labels_;

    return uri.str();
}

int PushGateway::PushInternal(HttpMethod method) {
    const auto serializer = TextSerializer{};

    auto metrics = collectable_->Collect();
    auto body = serializer.Serialize(metrics);
    auto uri = GetUri();
    auto statusCode = curlWrapper_->PerformHttpRequest(method, uri, body);

    if (statusCode < 100 || statusCode >= 400) {
        INFO("Invalid push metrics code: " << statusCode);
        return statusCode;
    }

    return 200;
}

std::unique_ptr<PushGateway> CreatePushGatewayFromConfig(Collectable* collectable, const config::Config& conf) {
    if (!conf.has_prometheus()) {
        return nullptr;
    }

    auto pushGatewayConf = conf.prometheus();

    Labels labels;
    for (int i = 0; i < pushGatewayConf.labels_size(); ++i) {
        labels.insert({pushGatewayConf.labels().at(i).key(), pushGatewayConf.labels().at(i).value()});
    }

    std::string username;
    if (pushGatewayConf.has_username()) {
        username = pushGatewayConf.username();
    }

    std::string password;
    if (pushGatewayConf.has_userpassword()) {
        username = pushGatewayConf.userpassword();
    }

    return std::make_unique<PushGateway>(collectable, pushGatewayConf.host(), pushGatewayConf.port(),
                                         pushGatewayConf.jobname(), labels, username, password);
}

} // namespace postboy::prometheus
