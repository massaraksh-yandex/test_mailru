find_package(PythonInterp 3 REQUIRED)

add_test(
    NAME integration
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/integration.py
    WORKING_DIRECTORY $<TARGET_FILE_DIR:builder>
)
