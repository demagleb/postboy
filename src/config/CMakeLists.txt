add_subdirectory(proto)

add_library(
    config
    config.cpp
)

target_link_libraries(
    config
    common_system
    common_util
    config_proto
    fmt
)
