set(READERS
    stream_reader
    string_reader
)

# set(WRITERS
#     stream_writer
#     string_writer
# )

function(create_executable folder)
    foreach(example ${ARGN})
        add_executable(${example} ${folder}/${example}.cpp)
        target_link_libraries(${example} ${PROJECT_NAME}_static)
        target_include_directories(${example} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    endforeach()
endfunction()

create_executable(reader ${READERS})
# create_executable(writer ${WRITERS})
