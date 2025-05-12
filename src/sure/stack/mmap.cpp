#include <sure/stack/mmap.hpp>

#include <utility>

namespace sure {

MmapStack::MmapStack(common::system::MmapAllocation allocation)
    : allocation_(std::move(allocation)) {
}

MmapStack MmapStack::AllocatePages(size_t count) {
    auto allocation = common::system::MmapAllocation::AllocatePages(count + 1);
    // Guard page
    allocation.ProtectPages(/*offset=*/0, /*count=*/1);
    return MmapStack{std::move(allocation)};
}

MmapStack MmapStack::AllocateBytes(size_t atLeast) {
    const size_t pageSize = common::system::MmapAllocation::PageSize();

    size_t pages = atLeast / pageSize;
    if (atLeast % pageSize != 0) {
        ++pages;
    }

    return MmapStack::AllocatePages(/*count=*/pages);
}

void* MmapStack::LowerBound() const {
    return allocation_.Start() + common::system::MmapAllocation::PageSize();
}

common::memory::MutableMemView MmapStack::MutView() {
    return allocation_.MutView();
}

MmapStack MmapStack::Acquire(common::memory::MutableMemView view) {
    return MmapStack(common::system::MmapAllocation::Acquire(view));
}

common::memory::MutableMemView MmapStack::Release() {
    return allocation_.Release();
}

} // namespace sure
