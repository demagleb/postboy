enable_testing()
include(GoogleTest)

add_custom_target(unit_tests)
add_custom_target(integration_tests)
add_custom_target(all_tests)
add_dependencies(all_tests unit_tests integration_tests)

function(_add_gtest TARGET_DEP TEST)
    add_executable(
        ${TEST}
        ${TEST}
    )
    target_link_libraries(
        ${TEST}
        ${ARGN}
        GTest::gtest_main
        GTest::gmock
    )

    gtest_discover_tests(${TEST})

    add_dependencies(${TARGET_DEP} ${TEST})
endfunction()

function(add_unit_test TEST)
    _add_gtest(unit_tests ${TEST} ${ARGN})
endfunction()

function(add_integration_test TEST)
    _add_gtest(integration_tests ${TEST} ${ARGN})
endfunction()

function(add_test_resource TARGET FILE)
    add_custom_command(
        TARGET ${TARGET} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
        $<TARGET_FILE_DIR:${TARGET}>/${FILE}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}
    )
endfunction()
