#pragma once

#include <sure/trampoline.hpp>

#include <common/memory/view.hpp>

// Switch between MachineContext-s
extern "C" void SwitchMachineContext(void* fromRsp, void* toRsp);

namespace sure {

// Target architecture: x86-64

struct MachineContext {
    void* rsp_;

    void Setup(common::memory::MutableMemView stack, ITrampoline* trampoline);

    void SwitchTo(MachineContext& target) {
        SwitchMachineContext(&rsp_, &target.rsp_);
    }

    void* TryStackPointer() const {
        return rsp_;
    }
};

} // namespace sure
