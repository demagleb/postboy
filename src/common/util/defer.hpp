#include <common/util/types.hpp>

#include <type_traits>
#include <utility>

namespace common::util::detail {

#define GENERATE_UNIQUE_ID(category) CONCAT(category, __COUNTER__)

template <class Func>
class DeferHelperRunner {
public:
    explicit DeferHelperRunner(Func&& func)
        : func_{std::move(func)} {
    }

    ~DeferHelperRunner() {
        func_();
    }

private:
    Func func_;
};

struct DeferHelperExpander {
    template <class Func>
    DeferHelperRunner<std::decay_t<Func>> operator||(Func&& func) const {
        return DeferHelperRunner<std::decay_t<Func>>{std::forward<Func>(func)};
    }
};

} // namespace common::util::detail

#define defer const auto GENERATE_UNIQUE_ID(defer_) = common::util::detail::DeferHelperExpander{} || [&]
