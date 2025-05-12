#pragma once

#include <common/util/assert.hpp>
#include <common/util/types.hpp>

#include <cstring>
#include <type_traits>
#include <utility>

namespace common::util {

template <typename>
class LightweightFunc;

template <typename Ret, typename... Args>
class LightweightFunc<Ret(Args...)> {
public:
    LightweightFunc() noexcept = default;

    LightweightFunc(const LightweightFunc&) noexcept = default;

    LightweightFunc(LightweightFunc&&) = delete;

    LightweightFunc& operator=(const LightweightFunc&) noexcept = default;

    LightweightFunc&& operator=(LightweightFunc&&) = delete;

    ~LightweightFunc() noexcept = default;

    template <typename Callable>
    LightweightFunc(const Callable& callable) noexcept {
        static_assert(!std::is_pointer_v<Callable>);
        static_assert(!std::is_function_v<Callable>);
        static_assert(sizeof(Callable) <= kFuncStorageSize);
        static_assert(std::is_trivially_copy_constructible_v<Callable>);
        static_assert(std::is_trivially_destructible_v<Callable>);

        rawFunction_ = &Trampoline<Callable>;
        memcpy(rawFunctionData_, &callable, sizeof(Callable));
    }

    inline explicit operator bool() const noexcept {
        return rawFunction_ != nullptr;
    }

    FORCE_INLINE
    Ret operator()(Args... args) const {
        VERIFY(rawFunction_ != nullptr, "Function is nullptr");
        return (*rawFunction_)(const_cast<char*>(rawFunctionData_), std::forward<Args>(args)...);
    }

private:
    template <typename Callable>
    static Ret Trampoline(char* data, Args... args) {
        auto* c = reinterpret_cast<Callable*>(data);
        return (*c)(std::forward<Args>(args)...);
    }

private:
    using FunctionPointerType = Ret (*)(char*, Args...);

    static constexpr size_t kFuncStorageSize = 2 * sizeof(void*);
    alignas(alignof(max_align_t)) char rawFunctionData_[kFuncStorageSize]{0};
    FunctionPointerType rawFunction_ = nullptr;
};

} // namespace common::util