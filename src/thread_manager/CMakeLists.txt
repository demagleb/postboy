add_library(
    thread_manager
    thread_manager.cpp
)

target_link_libraries(
    thread_manager
    common_util
    fmt
    config_proto
    pthread
    robin_map
)
