cmake_minimum_required(VERSION 3.15)
project(yadro3)

set(CMAKE_CXX_STANDARD 20)

set(COMMON_SOURCES
        TapeSorter.cpp
        FileTape.cpp
        TapeSorter.h
        FileTape.h
        TapeConfig.h
        ITape.h
)

# Console app
add_executable(tape_sort
        main.cpp
        ${COMMON_SOURCES}
)

# Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Google test app
add_executable(run_tests
        TapeSorterTest.cpp
        ${COMMON_SOURCES}
)

target_link_libraries(run_tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(run_tests)