#include "registry.hpp"

namespace postboy {

const ProcessorRegistry::Factory& ProcessorRegistry::GetProcessorFactory(config::Process::ProcessesCase p) {
    return processors_[p];
}

ProcessorRegistry* ProcessorRegistry::Instance() {
    static postboy::ProcessorRegistry instance;
    return &instance;
}

} // namespace postboy
