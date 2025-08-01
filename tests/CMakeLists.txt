cmake_minimum_required(VERSION 3.16)
project(TinyOS_Tests C)

set(UNITY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Unity)
set(UNITY_SRC ${UNITY_DIR}/src/unity.c)
set(UNITY_INCLUDE ${UNITY_DIR}/src)

if(EXISTS ${UNITY_SRC})
    set(TEST_SOURCES
        test_memory.c
        test_scheduler.c
        test_ipc.c
    )

    add_executable(tests ${TEST_SOURCES} ${UNITY_SRC})

    target_include_directories(tests PRIVATE
        ${UNITY_INCLUDE}
        ${CMAKE_SOURCE_DIR} 
    )

    target_link_libraries(tests pthread)

    enable_testing()
    add_test(NAME TinyOSTests COMMAND tests)
else()
    message(WARNING "Unity framework not found — skipping tests build.")
endif()
