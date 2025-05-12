add_library(
    websocket_client
    client.cpp
)

target_link_libraries(
    websocket_client
    common_concurrency
    common_util
    ev
    robin_map
    websockets
)
