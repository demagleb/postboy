set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_third_party(
    gtest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    GIT_PROGRESS TRUE
)
