include(cmake/ThirdParty.cmake)
set(protobuf_BUILD_TESTS OFF CACHE INTERNAL "")
set(protobuf_WITH_ZLIB OFF CACHE BOOL "" FORCE)
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
set(protobuf_ABSL_PROVIDER "package" CACHE STRING "" FORCE)
add_third_party(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG        v21.12
    GIT_SHALLOW    TRUE
    GIT_PROGRESS TRUE
)
set(Protobuf_LIBRARIES protobuf::libprotobuf)
get_target_property(Protobuf_INCLUDE_DIR protobuf::libprotobuf
  INTERFACE_INCLUDE_DIRECTORIES)
include(FindProtobuf)

function(generate_proto_cpp TARGET)
    set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
    set(PROTOBUF_GENERATE_CPP_APPEND_PATH OFF)
    protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${ARGN})
    add_library(${TARGET} ${PROTO_SRCS})
    target_link_libraries(${TARGET} protobuf::libprotobuf)
    target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    target_compile_options(${TARGET} PUBLIC -Wno-error -Wno-all -Wno-extra -Wno-pedantic)
endfunction()
