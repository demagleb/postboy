add_library(
    postboy_prepared
    prepared.cpp
)

target_link_libraries(
    postboy_prepared
    fmt
    postboy_base
    config_proto
)
