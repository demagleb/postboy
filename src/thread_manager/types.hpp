#include <common/util/tagged.hpp>

#include <optional>
#include <vector>

namespace thread_manager::types {

using CpuCoreList = std::vector<int>;

// Two different priorities should be tagged
struct PriorityTag;
struct NiceTag;

using ThreadPriority = common::util::Tagged<PriorityTag, int>;
using ThreadNice = common::util::Tagged<NiceTag, int>;

struct ThreadModel {
    std::optional<CpuCoreList> coreList;
    std::optional<ThreadPriority> priority;
    std::optional<ThreadNice> nice;
};

} // namespace thread_manager::types
