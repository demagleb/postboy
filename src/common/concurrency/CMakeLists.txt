add_library(
    common_concurrency
    locker.cpp
    moodycamel_readerwriter_queue.cpp
    moodycamel_concurrent_queue.cpp
    mpsc_unbounded_intrusive_lock_free_queue.cpp
    mpsc_unbounded_intrusive_lock_free_stack.cpp
    pointer_swap.cpp
    spinlock.cpp
    spsc_byte_queue.cpp
    spsc_queue.cpp
    wait_group.cpp
)

target_link_libraries(
    common_concurrency
    common_util
)
