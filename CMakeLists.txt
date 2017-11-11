cmake_minimum_required(VERSION 3.6)

project(mailru)

enable_testing()


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror")

add_definitions(-DBOOST_LOG_DYN_LINK=1)


find_package(Boost
    COMPONENTS system filesystem thread unit_test_framework program_options log log_setup
    REQUIRED
)


include_directories(${CMAKE_SOURCE_DIR}/include)


add_subdirectory(src)
add_subdirectory(bin)
add_subdirectory(tests)
