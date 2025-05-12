option(SURE_DEVELOPER "Sure development mode" OFF)
option(SURE_EXCEPTIONS "Sure exceptions context" ON)
option(SURE_UCONTEXT "Use ucontext" OFF)

message(STATUS "Host system processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(SURE_MACHINE_CONTEXT "no")

if((CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "aarch64") OR (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm") OR (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64"))
    set(SURE_MACHINE_CONTEXT "armv8_a_64")
endif()

if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(SURE_MACHINE_CONTEXT "x86_64")
endif()

if(SURE_UCONTEXT)
    set(SURE_MACHINE_CONTEXT "ucontext")
endif()

message(INFO "MachineContext: ${SURE_MACHINE_CONTEXT}")
message(INFO "ExceptionsContext: ${SURE_EXCEPTIONS_CONTEXT}")

get_filename_component(SURE_INCLUDE_PATH ".." ABSOLUTE)
get_filename_component(SURE_PATH "." ABSOLUTE)

set(
    SURE_SOURCES
    ${SURE_PATH}/context.hpp
    ${SURE_PATH}/context.cpp
    ${SURE_PATH}/machine/context.hpp
    ${SURE_PATH}/sanitizer/context.hpp
    ${SURE_PATH}/exceptions/context.hpp
    ${SURE_PATH}/stack.hpp
    ${SURE_PATH}/trampoline.hpp
    ${SURE_PATH}/stack/mmap.hpp
    ${SURE_PATH}/stack/mmap.cpp
    ${SURE_PATH}/stack/thread.hpp
    ${SURE_PATH}/stack/thread.cpp
)

# Machine context

set(
    SURE_MACHINE_CONTEXT_SOURCES
    ${SURE_PATH}/machine/${SURE_MACHINE_CONTEXT}/context.hpp
    ${SURE_PATH}/machine/${SURE_MACHINE_CONTEXT}/context.cpp
    ${SURE_PATH}/machine/${SURE_MACHINE_CONTEXT}/context.S
)

# Exceptions context

set(
    SURE_EXCEPTIONS_CONTEXT_SOURCES
    ${SURE_PATH}/exceptions/${SURE_EXCEPTIONS_CONTEXT}/context.hpp
    ${SURE_PATH}/exceptions/${SURE_EXCEPTIONS_CONTEXT}/context.cpp
)

enable_language(ASM)

add_library(
    sure STATIC
    ${SURE_SOURCES}
    ${SURE_MACHINE_CONTEXT_SOURCES}
    ${SURE_EXCETIONS_CONTEXT_SOURCES}
)

target_include_directories(sure PUBLIC ${SURE_INCLUDE_PATH})

target_link_libraries(sure PUBLIC common_memory pthread)

target_compile_definitions(sure PUBLIC SURE_MACHINE_CONTEXT_${SURE_MACHINE_CONTEXT})
target_compile_definitions(sure PUBLIC SURE_EXCEPTIONS_CONTEXT_${SURE_EXCEPTIONS_CONTEXT})
