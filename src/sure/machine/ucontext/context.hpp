#pragma once

#include <sure/trampoline.hpp>

#include <common/memory/view.hpp>

#include <ucontext.h>

namespace sure {

struct MachineContext {
    ucontext_t context;

    MachineContext();

    void Setup(wheels::MutableMemView stack, ITrampoline* trampoline);
    void SwitchTo(MachineContext& target);

    void* TryStackPointer() const {
        return nullptr; // Unknown
    }
};

} // namespace sure
