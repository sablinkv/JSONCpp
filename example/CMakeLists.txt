set(READERS
    stream_reader
    #string_reader
)

# set(WRITERS
#     stream_writer
#     string_writer
# )

function(create_executable folder examples)
    foreach(example ${examples})
        add_executable(${example} ${folder}/${example}.cpp)
        target_link_libraries(${example} ${PROJECT_NAME}_static)
        target_include_directories(${example} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    endforeach(example ${examples})
endfunction(create_executable )

create_executable(reader ${READERS})
# create_executable(writer ${WRITERS})
