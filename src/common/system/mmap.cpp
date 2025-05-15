#include "mmap.hpp"

#include <common/system/page_size.hpp>
#include <common/util/assert.hpp>

#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <sys/mman.h>

namespace common::system {

#define CHECK_RESULT(ret, error) VERIFY(ret != -1, error << " (errno = " << errno << ", " << strerror(errno) << ")")

//////////////////////////////////////////////////////////////////////

static size_t PagesToBytes(size_t count) {
    return count * PageSize();
}

size_t MmapAllocation::PageSize() {
    return PageSize();
}

MmapAllocation MmapAllocation::AllocatePages(size_t count, void* hint) {
    size_t size = PagesToBytes(count);

    void* start = mmap(/*addr=*/hint, /*length=*/size,
                       /*prot=*/PROT_READ | PROT_WRITE,
                       /*flags=*/MAP_PRIVATE | MAP_ANONYMOUS,
                       /*fd=*/-1, /*offset=*/0);

    VERIFY(start != MAP_FAILED, "Cannot allocate " << count << " pages");

    return MmapAllocation{(char*)start, size};
}

MmapAllocation MmapAllocation::Acquire(common::memory::MutableMemView view) {
    // TODO: check size and alignment
    return MmapAllocation{view.Data(), view.Size()};
}

void MmapAllocation::ProtectPages(size_t startIndex, size_t count) {
    int ret = mprotect(/*addr=*/(void*)(start_ + PagesToBytes(startIndex)),
                       /*len=*/PagesToBytes(count),
                       /*prot=*/PROT_NONE);
    CHECK_RESULT(ret, "Cannot protect pages [" << startIndex << ", " << startIndex + count << ")");
}

MmapAllocation::MmapAllocation(MmapAllocation&& that) {
    start_ = that.start_;
    size_ = that.size_;
    that.Reset();
}

MmapAllocation& MmapAllocation::operator=(MmapAllocation&& that) {
    Deallocate();
    start_ = that.start_;
    size_ = that.size_;
    that.Reset();
    return *this;
}

void MmapAllocation::Deallocate() {
    if (start_ == nullptr) {
        return;
    }

    int ret = munmap((void*)start_, size_);
    CHECK_RESULT(ret, "Cannot unmap allocated pages");
}

common::memory::MutableMemView MmapAllocation::Release() {
    auto view = MutView();
    Reset();
    return view;
}

void MmapAllocation::Reset() {
    start_ = nullptr;
    size_ = 0;
}

} // namespace common::system
