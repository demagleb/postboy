#pragma once

namespace postboy {

class IQueueBase {
public:
    virtual ~IQueueBase() = default;
};

enum class QueueType { SPSC, MPMC, UNKNOWN };

} // namespace postboy
