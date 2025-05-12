#include <sure/machine/x86_64/context.hpp>

//////////////////////////////////////////////////////////////////////

extern "C" void* SetupMachineContext(void* stack, void* trampoline, void* arg);

//////////////////////////////////////////////////////////////////////

namespace sure {

// https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64/
void MachineContextTrampoline(void*, void*, void*, void*, void*, void*, void* arg7) {
    ITrampoline* t = (ITrampoline*)arg7;
    t->Run();
}

//////////////////////////////////////////////////////////////////////

void MachineContext::Setup(common::memory::MutableMemView stack, ITrampoline* trampoline) {
    rsp_ = SetupMachineContext((void*)stack.End(), (void*)MachineContextTrampoline, trampoline);
}

} // namespace sure
