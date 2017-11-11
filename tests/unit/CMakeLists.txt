function(add_my_test name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} ${Boost_LIBRARIES} common builderlib)
    add_test(
        NAME ${name}
        COMMAND ${name}
    )
endfunction()


add_my_test(read_chunk_test ${CMAKE_CURRENT_SOURCE_DIR}/read_chunk_test.cc)
add_my_test(markov_test ${CMAKE_CURRENT_SOURCE_DIR}/markov_test.cc)
add_my_test(insert_to_markov_test ${CMAKE_CURRENT_SOURCE_DIR}/insert_to_markov_test.cc)
