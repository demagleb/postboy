#include <concepts>
#include <string>
#include <type_traits>
#include <utility>

namespace common::util {

template <class Op, class... Args>
void ApplyToMany(Op op, Args&&... args) {
    [[maybe_unused]] int dummy[] = {((void)op(std::forward<Args>(args)), 0)...};
}

template <class T>
concept EvaulateInsideToString = requires(T&& t) { std::to_string(t); };

template <class T>
concept HasToString = requires(T&& t) { t.ToString(); };

template <class T>
concept HasStringConstructor = requires(T&& t) { std::string(t); };

template <template <typename, typename...> class Container, typename Value, typename... Params>
    requires HasToString<Value>
std::string Join(const Container<Value, Params...>& container, char delimiter = ',') {
    std::string res;
    for (const Value& t : container) {
        if (res.empty()) {
            res += t.ToString();
        } else {
            res += delimiter + t.ToString();
        }
    }
    return res;
}

template <template <typename, typename...> class Container, typename Value, typename... Params>
    requires EvaulateInsideToString<Value>
std::string Join(const Container<Value, Params...>& container, char delimiter = ',') {
    std::string res;
    for (const Value& t : container) {
        if (res.empty()) {
            res += std::to_string(t);
        } else {
            res += delimiter + std::to_string(t);
        }
    }
    return res;
}

template <template <typename, typename...> class Container, typename Value, typename... Params>
    requires HasStringConstructor<Value>
std::string Join(const Container<Value, Params...>& container, char delimiter = ',') {
    std::string res;
    for (const Value& t : container) {
        if (res.empty()) {
            res += std::string(t);
        } else {
            res += delimiter + std::string(t);
        }
    }
    return res;
}

} // namespace common::util
