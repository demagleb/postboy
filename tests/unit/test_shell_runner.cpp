#include <gtest/gtest.h>

#include <common/shell_runner/runner.hpp>

TEST(TestShellRunner, CreateSimpleProcessSuccess) {
    common::shell_runner::Params params("/usr/bin/echo", {"123"});
    common::shell_runner::Runner shellRunner(params);
    auto status = std::move(shellRunner).Run();

    while (status.CheckStatus() == common::shell_runner::StatusChecker::Status::Running) {
    }

    EXPECT_EQ(status.CheckStatus(), common::shell_runner::StatusChecker::Status::Success);
}

TEST(TestShellRunner, CreateSimpleProcessFailure) {
    common::shell_runner::Params params("/usr/bin/echoo", {"123"});
    common::shell_runner::Runner shellRunner(params);
    auto status = std::move(shellRunner).Run();

    while (status.CheckStatus() == common::shell_runner::StatusChecker::Status::Running) {
    }

    EXPECT_EQ(status.CheckStatus(), common::shell_runner::StatusChecker::Status::Error);
}