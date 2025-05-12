add_executable(
    benchmark_spsc_queue
    benchmark_spsc_queue.cpp
)

target_link_libraries(
    benchmark_spsc_queue
    benchmark::benchmark
    common_concurrency
)


add_executable(
    benchmark_spsc_byte_queue
    benchmark_spsc_byte_queue.cpp
)

target_link_libraries(
    benchmark_spsc_byte_queue
    benchmark::benchmark
    common_concurrency
)

add_executable(
    benchmark_postboy_spsc_queue
    benchmark_postboy_spsc_queue.cpp
)

target_link_libraries(
    benchmark_postboy_spsc_queue
    benchmark::benchmark
    postboy_base_queues
)

add_executable(
    benchmark_postboy_mpmc_queue
    benchmark_postboy_mpmc_queue.cpp
)

target_link_libraries(
    benchmark_postboy_mpmc_queue
    benchmark::benchmark
    postboy_base_queues
)

add_executable(
    benchmark_boost_spsc_queue
    benchmark_boost_spsc_queue.cpp
)

target_link_libraries(
    benchmark_boost_spsc_queue
    benchmark::benchmark
    boost::boost
    common_concurrency
)

add_executable(
    benchmark_moodycamel_spsc_queue
    benchmark_moodycamel_spsc_queue.cpp
)

target_link_libraries(
    benchmark_moodycamel_spsc_queue
    benchmark::benchmark
    common_concurrency
)

add_executable(
    benchmark_moodycamel_mpmc_queue
    benchmark_moodycamel_mpmc_queue.cpp
)

target_link_libraries(
    benchmark_moodycamel_mpmc_queue
    benchmark::benchmark
    common_concurrency
)

add_executable(
    benchmark_boost_mpmc_queue
    benchmark_boost_mpmc_queue.cpp
)

target_link_libraries(
    benchmark_boost_mpmc_queue
    benchmark::benchmark
    boost::boost

)
