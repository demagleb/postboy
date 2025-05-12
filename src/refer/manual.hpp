#pragma once

#include <refer/unmanaged.hpp>

#include <cassert>
#include <type_traits>

namespace refer {

template <typename T>
class ManualLifetime {
    static_assert(std::is_base_of_v<Unmanaged, T>);

public:
    ManualLifetime() = default;

    // Non-copyable
    ManualLifetime(ManualLifetime&) = delete;
    ManualLifetime& operator=(const ManualLifetime&) = delete;

    ~ManualLifetime() {
        if (Allocated()) {
            DoDeallocate();
        }
    }

    bool Allocated() const {
        return allocated_;
    }

    template <typename... Args>
    Ref<T> Allocate(Args&&... args) {
        assert(!Allocated());
        T* obj = new (storage_) T(std::forward<Args>(args)...);
        allocated_ = true;
        return Adopt(obj);
    }

    // Precondition: Allocated() == true
    Ref<T> Get() {
        assert(Allocated());
        return Adopt(AsT());
    }

    // Precondition: Allocated() == true
    void Deallocate() {
        assert(Allocated());
        DoDeallocate();
    }

private:
    void DoDeallocate() {
        AsT()->~T();
        allocated_ = false;
    }

    T* AsT() {
        return reinterpret_cast<T*>(storage_);
    }

private:
    alignas(T) char storage_[sizeof(T)];
    bool allocated_ = false;
};

} // namespace refer
