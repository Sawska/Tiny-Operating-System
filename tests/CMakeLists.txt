cmake_minimum_required(VERSION 3.16)
project(TinyOS_Tests C)

# Go up to the project root
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Path to Unity
set(UNITY_DIR ${PROJECT_ROOT}/Unity)
set(UNITY_SRC ${UNITY_DIR}/src/unity.c)
set(UNITY_INCLUDE ${UNITY_DIR}/src)

# Only build tests if Unity exists
if(EXISTS ${UNITY_SRC})
    # Test source files
    set(TEST_SOURCES
        test_memory.c
        ../../memory.c
        ../../uart.c
    )

    # Create the tests executable
    add_executable(tests ${TEST_SOURCES} ${UNITY_SRC})

    # Include paths
    target_include_directories(tests PRIVATE
        ${UNITY_INCLUDE}
        ${PROJECT_ROOT}  # include main project headers
    )

    # Link pthread for threading support
    target_link_libraries(tests pthread)

    # Enable CTest
    enable_testing()
    add_test(NAME TinyOSTests COMMAND tests)
else()
    message(WARNING "Unity framework not found — skipping tests build.")
endif()
