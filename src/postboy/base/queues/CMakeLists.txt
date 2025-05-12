add_library(
    postboy_base_queues
    queue.cpp
    mpmc.cpp
    spsc.cpp
)

target_link_libraries(
    postboy_base_queues
    common_concurrency
    common_util
    boost::boost
)
