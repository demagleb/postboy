#include "api.hpp"
#include <binding.pb.h>
#include <postboy.pb.h>
#include <postboy/base/postboy.hpp>

#include <httplib.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>
#include <magic_enum/magic_enum.hpp>

#include <nlohmann/json.hpp>
#include <functional>

namespace postboy::api {

namespace {

const std::string kJsonContentType = "application/json";

} // namespace

class PostboyApi {
public:
    explicit PostboyApi(Runtime& runtime)
        : runtime_(runtime) {
    }

    void Init(httplib::Server& srv) {
        srv.Get("/config", Handler(&PostboyApi::GetConfig));

        srv.Get("/postboy/:postboyName/config", Handler(&PostboyApi::GetPostboyConfig));

        srv.Get("/postboy/:postboyName/state", Handler(&PostboyApi::GetPostboyState));

        srv.Post("/postboy/:postboyName/requestRun", Handler(&PostboyApi::PostPostboyRun));

        srv.Post("/postboy/:postboyName/requestStop", Handler(&PostboyApi::PostPostboyRequestStop));

        srv.Post("/postboy/:postboyName/requestResume", Handler(&PostboyApi::PostPostboyRequestResume));

        srv.Post("/postboy/:postboyName/requestFinish", Handler(&PostboyApi::PostPostboyRequestFinish));

        srv.Post("/postboy/:postboyName/requestSample", Handler(&PostboyApi::PostPostboyRequestSample));

        srv.Get("/postboy/:postboyName/latencies", Handler(&PostboyApi::GetPostboyLatencies));

        srv.Post("/postboy/", Handler(&PostboyApi::PostPostboy));

        srv.Post("/binding/", Handler(&PostboyApi::PostBinding));
    }

private:
    httplib::Server::Handler Handler(auto method) {
        using namespace std::placeholders;
        return std::bind(method, this, _1, _2);
    }

    void GetConfig(const httplib::Request&, httplib::Response& res) {
        std::string json;
        google::protobuf::util::MessageToJsonString(runtime_.Config(), &json);
        res.set_content(std::move(json), kJsonContentType);
    }

    void GetPostboyConfig(const httplib::Request& req, httplib::Response& res) {
        auto postboy = runtime_.Postboy(req.path_params.at("postboyName"));
        if (!postboy) {
            res.status = 404;
            return;
        }

        std::string content;
        google::protobuf::util::MessageToJsonString(postboy->get().Config(), &content);
        res.set_content(std::move(content), kJsonContentType);
    }

    void GetPostboyState(const httplib::Request& req, httplib::Response& res) {
        auto postboy = runtime_.Postboy(req.path_params.at("postboyName"));
        if (!postboy) {
            res.status = 404;
            return;
        }
        nlohmann::json json;
        json["state"] = magic_enum::enum_name(postboy->get().State());

        std::string content = json.dump();
        res.set_content(std::move(content), kJsonContentType);
    }

    void PostPostboyRequestStop(const httplib::Request& req, httplib::Response& res) {
        auto postboy = runtime_.Postboy(req.path_params.at("postboyName"));
        if (!postboy) {
            res.status = 404;
            return;
        }
        postboy->get().RequestStop();
    }

    void PostPostboyRequestResume(const httplib::Request& req, httplib::Response& res) {
        auto postboy = runtime_.Postboy(req.path_params.at("postboyName"));
        if (!postboy) {
            res.status = 404;
            return;
        }
        postboy->get().RequestResume();
    }

    void PostPostboyRequestFinish(const httplib::Request& req, httplib::Response& res) {
        auto postboy = runtime_.Postboy(req.path_params.at("postboyName"));
        if (!postboy) {
            res.status = 404;
            return;
        }
        postboy->get().RequestFinish();
    }

    void PostPostboyRequestSample(const httplib::Request& req, httplib::Response& res) {
        auto postboy = runtime_.Postboy(req.path_params.at("postboyName"));
        if (!postboy) {
            res.status = 404;
            return;
        }
        postboy->get().RequestSample(std::chrono::seconds(10));
    }

    void GetPostboyLatencies(const httplib::Request& req, httplib::Response& res) {
        auto postboy = runtime_.Postboy(req.path_params.at("postboyName"));
        if (!postboy) {
            res.status = 404;
            return;
        }
        auto latencies = postboy->get().GetLatencies();
        std::sort(latencies.begin(), latencies.end());

        nlohmann::json json;

        size_t size = latencies.size();
        json["p50"] = size == 0 ? nullptr : nlohmann::json(latencies[size * 0.5]);
        json["p90"] = size == 0 ? nullptr : nlohmann::json(latencies[size * 0.9]);
        json["p95"] = size == 0 ? nullptr : nlohmann::json(latencies[size * 0.95]);
        json["p99"] = size == 0 ? nullptr : nlohmann::json(latencies[size * 0.99]);
        json["p100"] = size == 0 ? nullptr : nlohmann::json(latencies.back());
        json["avg"] =
            size == 0 ? nullptr : nlohmann::json(std::accumulate(latencies.begin(), latencies.end(), 0ULL) / size);

        std::string content = json.dump();
        res.set_content(std::move(content), kJsonContentType);
    }

    void PostPostboy(const httplib::Request& req, httplib::Response& res) {
        config::Postboy config;
        auto status = google::protobuf::util::JsonStringToMessage(req.body, &config);
        if (!status.ok()) {
            res.status = 400;
            res.set_content(status.message().ToString(), "text/plain");
            return;
        }
        if (runtime_.Postboy(config.name())) {
            res.status = 409;
            return;
        }
        runtime_.CreatePostboy(config);
    }

    void PostBinding(const httplib::Request& req, httplib::Response& res) {
        config::Binding config;
        if (!google::protobuf::util::JsonStringToMessage(req.body, &config).ok()) {
            res.status = 400;
            return;
        }
        if (runtime_.Binding(config.name())) {
            res.status = 409;
            return;
        }
        runtime_.CreateBinding(config);
    }

    void PostPostboyRun(const httplib::Request& req, httplib::Response& res) {
        auto postboy = runtime_.Postboy(req.path_params.at("postboyName"));
        if (!postboy) {
            res.status = 404;
            return;
        }
        if (postboy->get().State() != PostboyState::Initialized) {
            res.status = 400;
            return;
        }
        postboy->get().Run();
    }

private:
    Runtime& runtime_;
};

void RunPostboyApi(Runtime& runtime) {
    PostboyApi api(runtime);

    httplib::Server srv;
    api.Init(srv);
    srv.set_logger([](const auto& req, const auto& res) {
        INFO("Request: " << req.method << " " << req.path << " " << req.version << " Responce: " << res.status);
    });
    srv.listen("0.0.0.0", 8080);
}

} // namespace postboy::api
