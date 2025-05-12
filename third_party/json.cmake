set(JSON_Install OFF CACHE INTERNAL "")
add_third_party(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
    GIT_PROGRESS TRUE
)
