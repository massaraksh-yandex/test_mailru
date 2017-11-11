add_executable(builder ${CMAKE_CURRENT_SOURCE_DIR}/builder.cc)
target_link_libraries(builder builderlib common ${Boost_LIBRARIES})

add_executable(complete ${CMAKE_CURRENT_SOURCE_DIR}/complete.cc)
target_link_libraries(complete common ${Boost_LIBRARIES})
