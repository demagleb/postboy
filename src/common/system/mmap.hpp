#pragma once

#include <common/memory/view.hpp>
#include <common/util/move_traits.hpp>

namespace common::system {

class MmapAllocation : public common::util::MoveOnly {
public:
    MmapAllocation() {
        Reset();
    }

    static size_t PageSize();

    // Allocate `count` pages of zeroed memory
    static MmapAllocation AllocatePages(size_t count, void* hint = nullptr);
    static MmapAllocation Acquire(memory::MutableMemView view);

    // Moving
    MmapAllocation(MmapAllocation&& that);
    MmapAllocation& operator=(MmapAllocation&& that);

    ~MmapAllocation() {
        Deallocate();
    }

    char* Start() const {
        return start_;
    }

    char* End() const {
        return start_ + size_;
    }

    size_t Size() const {
        return size_;
    }

    size_t PageCount() const {
        return Size() / PageSize();
    }

    memory::ConstMemView View() const {
        return {start_, size_};
    }

    memory::MutableMemView MutView() {
        return {start_, size_};
    }

    // Protect range of pages
    // Protected pages cannot be read, written or executed
    void ProtectPages(size_t startIndex, size_t count);

    void Deallocate();

    memory::MutableMemView Release();

private:
    MmapAllocation(char* start, size_t size)
        : start_(start)
        , size_(size) {
    }

    void Reset();

private:
    // Aligned to page boundary
    char* start_;
    size_t size_;
};

} // namespace common::system
