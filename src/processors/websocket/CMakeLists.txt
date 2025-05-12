add_library(
    websocket_metrics
    metrics.cpp
)

target_link_libraries(
    websocket_metrics

    postboy_base
    postboy_prometheus_metrics
)

add_processor(
    websocket.cpp

    common_util
    ev
    fmt
    postboy_base
    websocket_client
    websocket_metrics
)
