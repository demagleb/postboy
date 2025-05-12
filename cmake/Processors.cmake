function(add_processor TARGET)
    add_library(
        ${TARGET}_processor
        OBJECT
        ${TARGET}
    )

    target_link_libraries(
        ${TARGET}_processor
        ${ARGN}
    )

    target_link_libraries(postboy ${TARGET}_processor)

    target_compile_options(${TARGET}_processor PUBLIC -pthread)
endfunction()
