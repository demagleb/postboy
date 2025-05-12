#include "panic.hpp"

#include <mutex>
#include <sstream>
#include <stdexcept>

namespace common::util {

namespace detail {

static std::mutex mutex;

void Panic(common::util::SourceLocation where, const std::string& error) {
    {
        std::lock_guard guard(mutex);
        std::cerr << "Panicked at " << where << ": " << error << std::endl;
    }

    std::abort();
}

void Throw(common::util::SourceLocation where, const std::string& error) {
    throw std::runtime_error((std::stringstream() << "Panicked at " << where << ": " << error).str());
}

void Info(common::util::SourceLocation where, const std::string& text) {
    std::cerr << "\033[4;37m[INFO AT]\033[0m " << where << ": " << text << "\n";
}

void Warn(common::util::SourceLocation where, const std::string& text) {
    std::cerr << "\033[1;31m[WARNING AT]\033[0m " << where << ": " << text << "\n";
}

} // namespace detail

} // namespace common::util
