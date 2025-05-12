#include <common/util/assert.hpp>
#include <common/util/panic.hpp>
#include <postboy/base/takeable_processor.hpp>
#include <postboy/base/registry.hpp>

#include <fmt/format.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>

namespace postboy {

namespace {

class FileWriter : public TakeableProcessor<std::string> {
public:
    void Process() override {
        auto inp = Take();
        if (inp) {
            out_ << *inp << "\n";
        } else {
            RequestFinish();
        }
    }

    void Init(const config::Process& config) override {
        ENSURE(config.Processes_case() == config::Process::kFileWriter,
               "Process case for [FileWriter] from config is not [kFileWriter]");

        INFO("[FileWriter]: Output file: " << config.filewriter().path());
        out_.open(config.filewriter().path());
        ASSERT(out_.is_open(), "Stream is not opened");
    }

    void Finalize() override {
        out_.close();
    }

private:
    std::ofstream out_{};
};

} // namespace

REGISTER_PROCESSOR(FileWriter, config::Process::ProcessesCase::kFileWriter);

} // namespace postboy
