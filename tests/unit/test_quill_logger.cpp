#include <gtest/gtest.h>

#include <common/system/file.hpp>

#include <chrono>
#include <thread>

#include <quill/Backend.h>
#include <quill/core/SinkManager.h>
#include <quill/Frontend.h>
#include <quill/Logger.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>
#include <quill/sinks/FileSink.h>
#include <quill/sinks/StreamSink.h>
#include <quill/std/Set.h>

TEST(QuilLogger, SimpleLog) {
    // Start the backend thread
    quill::Backend::start();

    // Frontend write to console
    auto consoleSink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("console_sink");
    quill::Logger* consoleLogger = quill::Frontend::create_or_get_logger("root1", std::move(consoleSink));

    LOG_INFO(consoleLogger, "This is a log info example {}", 123);

    // Frontend write to file
    auto fileSink = quill::Frontend::create_or_get_sink<quill::FileSink>(
        "test_quill_logger_output.txt",
        []() {
            quill::FileSinkConfig cfg;
            cfg.set_open_mode('w');
            cfg.set_write_buffer_size(4096);
            return cfg;
        }(),
        quill::FileEventNotifier{});

    quill::Logger* fileLogger = quill::Frontend::create_or_get_logger("root2", std::move(fileSink));

    std::set<int> value{1, 2, 5};
    for (int i = 0; i < 1024; ++i) {
        LOG_INFO(fileLogger, "Logging std set {}", value);
    }

    // Wait for flush from backend
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::string logFileResult = common::system::ReadFile<std::string>("test_quill_logger_output.txt");
    EXPECT_NE(logFileResult.find("Logging std set {1, 2, 5}"), std::string::npos);
}
