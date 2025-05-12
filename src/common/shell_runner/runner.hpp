#pragma once

#include <unistd.h>
#include <vector>
#include <string>
#include <chrono>

#include <spawn.h>

namespace common::shell_runner {

struct Params {
    using Args = std::vector<std::string>;
    using Cmd = std::string;

    Params(Cmd cmd, Args args)
        : command_(std::move(cmd))
        , args_(std::move(args)) {
        argv_.reserve(args_.size());
        for (auto& arg : args_) {
            argv_.push_back(arg.data());
        }
    }

    const char* Command() {
        return command_.c_str();
    }

    char* const* Argv() {
        return reinterpret_cast<char* const*>(argv_.data());
    }

    pid_t* Pid() {
        return &pid_;
    }

private:
    Cmd command_;
    Args args_;
    pid_t pid_;
    std::vector<char*> argv_;
};

struct Result {
    int exitCode;
    std::string stdOut;
    std::string stdErr;
};

class StatusChecker {
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    enum class Status { Running, Success, Error };

    StatusChecker(Status status, const std::string& error, const Params params);

    Status CheckStatus();

    std::string ErrorMsg() const {
        return error_;
    }

private:
    // todo make as parameter
    const int kMaxProcessingTime_ = 20;

    Status status_;
    std::string error_{};
    TimePoint launchTime_;
    Params params_;
};

class Runner {
public:
    explicit Runner(Params);

    StatusChecker Run() &&;

private:
    Params params_;
};

} // namespace common::shell_runner
