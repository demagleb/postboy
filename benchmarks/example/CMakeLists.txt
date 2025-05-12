add_executable(
    benchmark_example
    benchmark_example.cpp
)

target_link_libraries(
    benchmark_example
    common_system
    benchmark::benchmark
)
