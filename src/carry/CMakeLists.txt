add_library(
    carry
    detail/entries.hpp
    detail/storage.hpp
    context.hpp
    context.cpp
    context-inl.hpp
    delayed.hpp
    delayed.cpp
    empty.hpp
    empty.cpp
    kv.hpp
    mutable.hpp
    mutable.cpp
    new.hpp
    wrap.hpp
    wrap.cpp
    carrier.hpp
    current.hpp
    current.cpp
    scope.hpp
    scope.cpp
)

target_link_libraries(carry PUBLIC compass refer common_util)
