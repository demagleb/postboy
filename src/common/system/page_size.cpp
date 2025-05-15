#include "page_size.hpp"

#include <unistd.h>

namespace common::system {

class PageSizeDetector {
public:
    PageSizeDetector() {
        page_size_ = DetectPageSize();
    }

    size_t GetPageSize() const {
        return page_size_;
    }

private:
    size_t DetectPageSize() {
        return sysconf(_SC_PAGESIZE);
    }

private:
    size_t page_size_;
};

size_t PageSize() {
    static PageSizeDetector page_size_detector;

    return page_size_detector.GetPageSize();
}

} // namespace common::system
