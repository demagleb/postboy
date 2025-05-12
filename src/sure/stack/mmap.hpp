#pragma once

#include <common/memory/view.hpp>
#include <common/system/mmap.hpp>

#include <cstdint>

namespace sure {

// MmapStack = mmap allocation with guard page

class MmapStack {
public:
    MmapStack() = delete;

    static MmapStack AllocateBytes(size_t atLeast);

    // Allocated memory will be released to the operating system
    ~MmapStack() = default;

    MmapStack(MmapStack&&) = default;
    MmapStack& operator=(MmapStack&&) = default;

    void* LowerBound() const;

    // Including guard page
    size_t AllocationSize() const {
        return allocation_.Size();
    }

    common::memory::MutableMemView MutView();

    // Release / acquire ownership for the underlying memory region
    [[nodiscard("Memory leak")]] common::memory::MutableMemView Release();
    static MmapStack Acquire(common::memory::MutableMemView view);

private:
    MmapStack(common::system::MmapAllocation allocation);

    static MmapStack AllocatePages(size_t count);

private:
    common::system::MmapAllocation allocation_;
};

} // namespace sure
