#include <carry/context.hpp>

#include <carry/empty.hpp>

namespace carry {

Context::Context()
    : storage_(Empty().StoragePrivate()) {
}

} // namespace carry
