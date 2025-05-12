add_library(
    postboy_runtime
    runtime.cpp
)

target_link_libraries(
    postboy_runtime
    postboy_base
    postboy_prepared
    postboy_prometheus_metrics
    config_proto
)
