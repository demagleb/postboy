add_subdirectory(proto)

add_library(
    postboy_prometheus_metrics
    check_names.cpp
    counter.cpp
    details/builder.cpp
    family.cpp
    gateway.cpp
    labels.cpp
    proto_serializer.cpp
    registry.cpp
    text_serializer.cpp
)

target_link_libraries(
    postboy_prometheus_metrics
    CURL::libcurl
    common_util
    config
    crypto
    proto_metrics_format
    robin_map
    ssl
)
