set(RAPIDJSON_BUILD_DOC         OFF CACHE BOOL "Builds rapidjson documentation"             FORCE)
set(RAPIDJSON_BUILD_EXAMPLES    OFF CACHE BOOL "Builds rapidjson examples"                  FORCE)
set(RAPIDJSON_BUILD_TESTS       OFF CACHE BOOL "Builds rapidjson perftests and unittests"   FORCE)
add_third_party(
    rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson
    GIT_TAG        v1.1.0
    GIT_PROGRESS TRUE
)
add_library(rapidjson INTERFACE)
target_include_directories(
    rapidjson
    SYSTEM INTERFACE
    ${rapidjson_SOURCE_DIR}/include
)
