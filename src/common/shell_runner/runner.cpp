#include "common/util/assert.hpp"
#include <bits/chrono.h>
#include <common/shell_runner/runner.hpp>

#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

namespace common::shell_runner {

Runner::Runner(Params params)
    : params_(std::move(params)) {
}

StatusChecker::StatusChecker(Status status, const std::string& error, const Params params)
    : status_(status)
    , error_(error)
    , launchTime_(std::chrono::system_clock::now())
    , params_(std::move(params)) {
}

StatusChecker Runner::Run() && {
    const auto res = posix_spawn(params_.Pid(),     /* pid */
                                 params_.Command(), /* path */
                                 nullptr,           /* file_actions */
                                 nullptr,           /* attrp */
                                 params_.Argv(),    /* argv */
                                 nullptr);          /* envp */

    if (res == 0) {
        return StatusChecker{StatusChecker::Status::Running, "", std::move(params_)};
    } else {
        char buffer[256];
        strerror_r(errno, buffer, 256);
        return StatusChecker{StatusChecker::Status::Error, std::string(buffer), std::move(params_)};
    }
}

StatusChecker::Status StatusChecker::CheckStatus() {
    if (status_ != Status::Running) {
        return status_;
    }

    const auto timeElapsed = Clock::now() - launchTime_;
    const auto timeElapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(timeElapsed).count();

    if (timeElapsedMinutes > kMaxProcessingTime_) {
        status_ = Status::Error;
        error_ = "Processing time reached " + std::to_string(kMaxProcessingTime_) + "m";
    }

    int pidStatus{0};
    const auto waitpidResult = waitpid(*params_.Pid(), &pidStatus, WNOHANG);

    if (waitpidResult == 0) {
        // Still running;
        return status_;
    }

    if (WIFEXITED(pidStatus)) {
        status_ = Status::Success;
        return status_;
    }

    status_ = Status::Error;
    char buffer[256];
    strerror_r(errno, buffer, 256);
    error_ = std::string(buffer);
    return status_;
}

} // namespace common::shell_runner
