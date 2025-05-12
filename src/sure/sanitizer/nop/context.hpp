#pragma once

#include <common/memory/view.hpp>

namespace sure {

struct SanitizerContext {
    void Setup(common::memory::MutableMemView) {
        // Nop
    }

    void AfterStart() {
        // Nop
    }

    void BeforeSwitch(SanitizerContext& /*target*/) {
        // Nop
    }

    void AfterSwitch() {
        // Nop
    }

    void BeforeExit(SanitizerContext& /*target*/) {
        // Nop
    }
};

} // namespace sure
