add_library(
    common_csv
    file_reader.cpp
    file_writer.cpp
    reader.cpp
    writer.cpp
)

target_link_libraries(
    common_csv
    robin_map
    boost::boost
)
