#include "api.hpp"

#include <common/system/file.hpp>
#include <config/config.h>

#include <postboy/base/metrics.hpp>
#include <postboy/notifiers/base_message_notifier/message_notifier.hpp>
#include <postboy/notifiers/telegram_notifier/telegram_notifier.hpp>
#include <postboy/runtime/runtime.hpp>
#include <postboy/prepared/prepared.hpp>
#include <postboy/prometheus_metrics/gateway.hpp>

#include <thread_manager/thread_manager.hpp>

#include <argparse/argparse.hpp>
#include <fmt/format.h>

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("postboy", "", argparse::default_arguments::help);

    program.add_argument("-c", "--config")
        .help("path to postboy config")
        .default_value(std::filesystem::path(config::kDefaultConfigPath))
        .metavar("path")
        .action([](const auto& stringPath) { return std::filesystem::path(stringPath); });

    program.add_argument("--control-api").help("run control api").default_value(false).implicit_value(true);

    program.parse_args(argc, argv);

    const auto configPath = program.get<std::filesystem::path>("-c");
    config::ValidatePath(configPath);
    INFO("postboy config [" << configPath << "]");

    auto config = std::make_shared<config::Config>(config::FromFS(configPath));
    postboy::Runtime runtime(std::move(config));
    runtime.Prepare();
    runtime.Run();

    if (program.get<bool>("--control-api")) {
        postboy::api::RunPostboyApi(runtime);
    }

    runtime.WaitFinished();
    return 0;
}
