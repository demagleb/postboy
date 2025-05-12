add_subdirectory(queues)

add_library(
    postboy_base
    binding.cpp
    metrics.cpp
    postboy.cpp
    putable_processor.cpp
    takeable_processor.cpp
    processor.cpp
)

target_link_libraries(
    postboy_base
    common_concurrency
    common_util
    fmt
    postboy_prometheus_metrics
    config_proto
    pthread
    thread_manager
    boost::boost
)

add_library(
    processors
    OBJECT

    registry.cpp
    registry.hpp
)

target_link_libraries(
    processors
    config_proto
    robin_map
    postboy_base
)
