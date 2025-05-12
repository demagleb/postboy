add_library(
    common_http
    async_request.cpp
    connection_pull.cpp
    sync_request.cpp
    types.cpp
)

target_link_libraries(
    common_http
    CURL::libcurl
    boost::boost
    common_concurrency
    common_util
    crypto
    ssl
)
