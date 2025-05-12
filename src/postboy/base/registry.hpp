#pragma once

#include <config/proto/process.pb.h>

#include <tsl/robin_map.h>

#include <functional>

namespace postboy {

class Processor;

class ProcessorRegistry {
public:
    using Factory = std::function<std::unique_ptr<Processor>()>;

public:
    template <typename T>
    void RegisterProcessor(config::Process::ProcessesCase p) {
        static_assert(std::is_base_of<Processor, T>::value);
        static_assert(std::is_default_constructible<T>::value);
        processors_[p] = []() { return std::make_unique<T>(); };
    }

    const Factory& GetProcessorFactory(config::Process::ProcessesCase p);

    static ProcessorRegistry* Instance();

public:
    tsl::robin_map<config::Process::ProcessesCase, Factory> processors_;
};

} // namespace postboy

#define REGISTER_PROCESSOR(CLASS, ENUM)                                             \
                                                                                    \
    namespace {                                                                     \
    struct P {                                                                      \
        inline P() {                                                                \
            postboy::ProcessorRegistry::Instance()->RegisterProcessor<CLASS>(ENUM); \
        }                                                                           \
    } p;                                                                            \
    }
