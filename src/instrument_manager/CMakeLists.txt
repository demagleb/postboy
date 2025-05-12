add_library(
    instrument_manager
    instrument_manager.cpp
)

target_link_libraries(
    instrument_manager
    common_csv
    common_util
    robin_map
)
