add_library(
    common_memory
    buffer.cpp
    view.cpp
    view_of.cpp
)

target_link_libraries(
    common_memory
    common_util
)
