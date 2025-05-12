#include <postboy/base/processor.hpp>
#include <postboy/base/putable_processor.hpp>
#include <postboy/base/registry.hpp>

#include <common/util/assert.hpp>
#include <common/util/panic.hpp>

#include <filesystem>
#include <fstream>

namespace postboy {

namespace {

class FileReader : public PutableProcessor<std::string> {
public:
    void Process() override {
        std::string line;
        if (std::getline(in_, line)) {
            Put(std::move(line));
        } else {
            RequestFinish();
        }
    }

    void Init(const config::Process& config) override {
        ENSURE(config.Processes_case() == config::Process::kFileReader,
               "Process case for [FileReader] from config is not [kFileReader]");
        ENSURE(std::filesystem::exists(config.filereader().path()), "File not exists: " << config.filereader().path());

        INFO("Input file: " << config.filereader().path());
        in_.open(config.filereader().path());
        ASSERT(in_.is_open(), "Stream is not opened");
    }

    void Finalize() override {
        in_.close();
    }

private:
    std::ifstream in_{};
};

} // namespace

REGISTER_PROCESSOR(FileReader, config::Process::ProcessesCase::kFileReader);

} // namespace postboy
