add_library(
    common_intrusive
    forward_list.cpp
    list.cpp
)

target_link_libraries(
    common_intrusive
    common_util
)
