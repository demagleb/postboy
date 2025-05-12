add_library(
    common_system
    binary_dir.cpp
    cpu_helper.cpp
    file.cpp
    high_resolution_clock.cpp
    page_size.cpp
)

target_link_libraries(
    common_system
    common_memory
    common_util
)
