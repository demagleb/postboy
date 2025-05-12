add_library(
    message_notifier
    message_notifier.cpp
)

target_link_libraries(
    message_notifier
    common_concurrency
    common_util
)
