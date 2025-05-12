#pragma once

#include <sure/trampoline.hpp>

#include <wheels/memory/view.hpp>

namespace sure {

struct MachineContext {
    MachineContext();

    void Setup(wheels::MutableMemView stack, ITrampoline* trampoline);
    void SwitchTo(MachineContext& target);
    void* TryStackPointer() const;
};

} // namespace sure
