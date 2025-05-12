add_subdirectory(concurrency)
add_subdirectory(csv)
add_subdirectory(http)
add_subdirectory(intrusive)
add_subdirectory(memory)
add_subdirectory(system)
add_subdirectory(util)
add_subdirectory(shell_runner)

add_library(common)

target_link_libraries(
    common
    common_concurrency
    common_csv
    common_http
    common_intrusive
    common_memory
    common_system
    common_time
    common_util
    common_shell_runner
)
