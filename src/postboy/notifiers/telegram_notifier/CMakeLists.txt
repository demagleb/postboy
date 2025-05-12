add_library(
    telegram_notifier
    telegram_notifier.cpp
)

target_link_libraries(
    telegram_notifier
    common_http
    common_util
    config
    fmt
    message_notifier
)
