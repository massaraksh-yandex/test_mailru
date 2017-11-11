set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/downloader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/service.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/markov.cc
)

add_library(builderlib STATIC "${SOURCES}")
