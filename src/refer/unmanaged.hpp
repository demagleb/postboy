#pragma once

#include <refer/ref.hpp>

namespace refer {

struct Unmanaged : virtual IManaged {
    void AddRef() override {
        // No-op
    }

    void ReleaseRef() override {
        // No-op
    }

    bool IsManualLifetime() const override {
        return true;
    }
};

} // namespace refer
