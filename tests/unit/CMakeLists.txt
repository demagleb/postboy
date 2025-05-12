add_unit_test(
    test_defer.cpp

    common_util
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_constexpr_array.cpp

    common_util
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_locker.cpp

    common_concurrency
    common_system
    pthread
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_high_resolution_clock.cpp

    common_system
    pthread
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_cpu_helper.cpp

    common_system
    pthread
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_spinlock.cpp

    common_concurrency
    common_system
    pthread
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_tagged.cpp

    common_util
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_strings.cpp

    common_util
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_binary_dir.cpp

    common_system
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_spsc_queue.cpp

    common_concurrency
    common_system
    common_util
    pthread
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_spsc_byte_queue.cpp

    common_concurrency
    common_system
    common_util
    pthread
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_csv.cpp

    common_csv
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_postboy_prometheus_metrics.cpp

    common_concurrency
    common_system
    common_util
    postboy_prometheus_metrics
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_lightweight_function.cpp

    common_util
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_carry.cpp

    carry
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_quill_logger.cpp

    common_system
    common_util
    quill::quill
)
# ---------------------------------------------------------------------------------- #
add_unit_test(
    test_shell_runner.cpp

    common_shell_runner
    common_system
)
